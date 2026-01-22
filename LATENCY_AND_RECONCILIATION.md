# XPilot Latency and Reconciliation Architecture

## Executive Summary

XPilot NG uses a **server-authoritative, lockstep-style architecture** with UDP-based frame synchronization. The game does **not** implement modern client-side prediction or server reconciliation techniques commonly used in contemporary multiplayer shooters. Instead, it relies on a deterministic frame-based update system where the server sends complete game state snapshots each frame, and clients display these updates directly.

---

## Architecture Overview

### Network Protocol

**Transport Layer: UDP**
- XPilot uses UDP datagrams for all game communication (see `netserver.c` lines 69-78)
- The code explicitly notes that TCP was too slow for real-time gameplay due to retransmission delays
- UDP provides low latency but requires custom reliability mechanisms for critical data

**Reliability Layer: Custom Acknowledgment System**
- Critical data (player names, map data, etc.) uses a custom reliable data stream piggybacked on UDP packets
- The server maintains acknowledgment timers and retransmits unacknowledged reliable data
- Uses byte-position-based acknowledgments with exponential backoff retransmission (see `netserver.c` lines 2404-2512)

### Frame Synchronization Model

**Lockstep-Style Updates**
- The server runs at a fixed frame rate (typically 12 FPS, configurable)
- Each frame, the server sends a complete snapshot of visible game state to each client
- Frame packets are marked with `PKT_START` and `PKT_END` containing frame numbers (see `frame.c` lines 2187-2247)

**No Client-Side Prediction**
- Clients do not predict their own movement or actions
- All game state comes directly from server updates
- Keyboard input is sent to the server, which processes it and includes results in the next frame update

**Frame Numbering and Ordering**
- Each frame has a unique `frame_loops` number
- Clients track `last_loops` to detect out-of-order or duplicate packets
- Out-of-order packets are dropped (see `netclient.c` lines 1051-1061)

---

## Latency Handling Mechanisms

### 1. Keyboard Input Handling

**Input Transmission**
- Clients send keyboard state changes with sequence numbers (`last_key_change`)
- Server tracks the last processed keyboard update per connection
- Server includes `last_key_change` in frame start packets to acknowledge input receipt (see `netserver.c` lines 2249-2283, 2203)

**No Input Buffering**
- The server processes keyboard input immediately when received
- No queuing or buffering of inputs for later application
- This means input lag equals network round-trip time plus one frame

### 2. Round-Trip Time (RTT) Estimation

**RTT Measurement**
- Server measures RTT using acknowledgment packets
- Uses smoothed RTT estimation with deviation tracking (see `netserver.c` lines 2514-2617)
- Based on Comer's "Internetworking with TCP/IP" algorithms
- Used primarily for reliable data retransmission timing, not gameplay latency compensation

**RTT Calculation:**
```c
rtt = main_loops - rel_loops;  // Round trip in server frames
rtt_smoothed += delta;  // Smoothed with 1/8 weight
rtt_dev += delta - (rtt_dev >> 2);  // Deviation tracking
rtt_retransmit = ((rtt_smoothed >> 2) + rtt_dev) >> 1;  // Retransmit timeout
```

### 3. Frame Buffering

**Client-Side Buffering**
- Clients maintain a receive window of frame buffers (configurable size)
- Oldest frames are dropped if the buffer fills up
- Prevents rendering lag when network temporarily slows (see `netclient.c` lines 1073-1200)

**No Interpolation or Extrapolation**
- Clients display frames exactly as received from the server
- No position interpolation between frames
- No velocity-based extrapolation for smooth movement

### 4. Adaptive Frame Rate

**Per-Player Frame Rate**
- Server can send frames at different rates to different clients
- Paused players receive updates at lower rates (`pausedFPS`, `waitingFPS` options)
- Clients can request specific frame rates via `PKT_ASYNC_FPS` packet
- Server throttles frame sending based on client capabilities (see `frame.c` lines 1173-1177)

---

## Reconciliation and Synchronization

### What XPilot Does NOT Have

**No Client-Side Prediction**
- Clients cannot predict their own movement
- All actions must wait for server confirmation
- This creates noticeable input lag, especially on high-latency connections

**No Server Reconciliation**
- No mechanism to rewind and replay client predictions
- No correction of client-predicted state when server state differs
- No snapshot interpolation system

**No Lag Compensation**
- Server does not "rewind time" for hit detection
- Collision detection uses current server state, not historical positions
- Players with higher latency are at a disadvantage in combat

### What XPilot DOES Have

**Deterministic State Updates**
- Server sends complete, authoritative game state each frame
- All clients see the same state (eventually)
- Frame numbers ensure ordering and detect duplicates

**Reliable Data Stream**
- Critical data (map, player info, messages) uses reliable transmission
- Acknowledgment-based retransmission ensures delivery
- Prevents desynchronization from lost packets

**State Change Tracking**
- Server tracks which clients have seen which state changes
- Uses connection masks (`conn_mask`) to avoid redundant updates
- Example: fuel stations, cannons, targets only send updates when state changes (see `update.c` lines 440-469, `frame.c` lines 468-563)

**Keyboard Input Sequencing**
- Inputs are sequenced to prevent processing duplicates
- Server ignores stale keyboard updates (`change <= connp->last_key_change`)
- Prevents input replay issues

---

## Comparison with Industry Standards

### Modern Multiplayer Shooter Techniques

**1. Client-Side Prediction (Industry Standard)**
- **Modern games**: Clients immediately apply player inputs locally, then send to server
- **XPilot**: Clients send inputs to server and wait for next frame update
- **Impact**: XPilot has higher perceived input lag

**2. Server Reconciliation (Industry Standard)**
- **Modern games**: Server sends authoritative state; clients rewind predictions and replay from server state
- **XPilot**: No prediction means no reconciliation needed (or possible)
- **Impact**: Simpler code, but no latency hiding

**3. Lag Compensation (Industry Standard)**
- **Modern games**: Server rewinds time during hit detection to account for player latency
- **XPilot**: Hit detection uses current server state
- **Impact**: Higher-latency players at significant disadvantage

**4. Snapshot Interpolation (Industry Standard)**
- **Modern games**: Clients interpolate between server snapshots for smooth rendering
- **XPilot**: Clients display discrete frames directly
- **Impact**: Movement can appear choppy, especially at low frame rates

**5. Entity Interpolation (Industry Standard)**
- **Modern games**: Other players' positions interpolated between updates
- **XPilot**: Other players' positions updated discretely each frame
- **Impact**: Other players may appear to "teleport" between frames

### Historical Context

**XPilot's Approach (1990s-2000s Era)**
- Similar to early multiplayer games (Doom, early Quake)
- Server-authoritative, frame-based updates
- Simple and reliable, but high latency sensitivity

**Modern Approach (2000s-Present)**
- Pioneered by QuakeWorld (1996) with client-side prediction
- Refined by Source engine (lag compensation)
- Standard in modern FPS games (Counter-Strike, Overwatch, Valorant)

### Why XPilot's Approach Was Acceptable

**1. Game Genre**
- Space combat game with slower movement than FPS
- Less twitch-based gameplay reduces impact of input lag
- Turn-based elements (autopilot, targeting) less latency-sensitive

**2. Network Conditions (1990s-2000s)**
- LAN play was common (low latency)
- Internet connections were slower but more consistent
- Less packet loss and jitter than modern wireless networks

**3. Technical Constraints**
- Simpler to implement and debug
- Lower CPU requirements (no prediction/reconciliation overhead)
- Deterministic behavior easier to verify

---

## Specific Code Examples

### Frame Update Transmission

```c
// Server sends frame start (frame.c:2187-2210)
int Send_start_of_frame(connection_t *connp)
{
    Sockbuf_clear(&connp->w);
    Packet_printf(&connp->w, "%c%ld%ld",
                  PKT_START, frame_loops, connp->last_key_change);
    // ... sends all game state ...
    Packet_printf(&connp->w, "%c%ld", PKT_END, frame_loops);
}
```

### Keyboard Input Processing

```c
// Server processes keyboard input (netserver.c:2249-2283)
static int Receive_keyboard(connection_t *connp)
{
    Packet_scanf(&connp->r, "%c%ld", &ch, &change);
    if (change <= connp->last_key_change)
        // Already processed, ignore
        return 1;
    connp->last_key_change = change;
    memcpy(pl->last_keyv, connp->r.ptr, size);
    Handle_keyboard(pl);  // Process immediately
}
```

### Client Frame Reception

```c
// Client receives and processes frames (netclient.c:1051-1061)
if (loop > last_loops) {
    frame->loops = loop;
    return 2;  // New frame, process it
} else {
    // Packet out of order. Drop it.
    // No attempt to reorder or interpolate
}
```

---

## Performance Characteristics

### Latency Breakdown

**Minimum Latency (LAN, 0ms RTT)**
- Input → Server: ~0ms
- Server Processing: ~83ms (1 frame at 12 FPS)
- Server → Client: ~0ms
- **Total: ~83ms** (noticeable but playable)

**Typical Internet (50ms RTT)**
- Input → Server: ~25ms
- Server Processing: ~83ms
- Server → Client: ~25ms
- **Total: ~133ms** (noticeable lag)

**High Latency (200ms RTT)**
- Input → Server: ~100ms
- Server Processing: ~83ms
- Server → Client: ~100ms
- **Total: ~283ms** (significant lag, poor gameplay)

### Bandwidth Usage

**Per-Frame Data**
- Frame start/end headers: ~10 bytes
- Player self data: ~50-100 bytes
- Visible ships: ~10-20 bytes each
- Visible shots/debris: ~2-4 bytes each
- Map state updates: variable
- **Typical frame: 500-2000 bytes**

**At 12 FPS: 6-24 KB/s per client**
- Acceptable for 1990s-2000s connections
- Modern games use 10-100x more bandwidth with interpolation/prediction

---

## Recommendations for Modernization

If XPilot were to be modernized, the following improvements would align it with industry standards:

### 1. Add Client-Side Prediction
- Clients predict their own movement immediately
- Send inputs to server while applying locally
- Reduces perceived input lag to near-zero

### 2. Implement Server Reconciliation
- Server sends authoritative state with timestamps
- Clients rewind predictions and replay from server state
- Smooth correction when predictions differ

### 3. Add Lag Compensation
- Server stores historical game states (last 100-200ms)
- During hit detection, rewind to when player saw target
- Fair combat regardless of latency

### 4. Implement Interpolation
- Clients buffer 2-3 server snapshots
- Interpolate entity positions between snapshots
- Smooth 60+ FPS rendering from 12 FPS server updates

### 5. Add Extrapolation
- For other players, extrapolate position from last known velocity
- Correct when authoritative update arrives
- Reduces perceived lag for other players' movements

---

## Conclusion

XPilot NG uses a **classic server-authoritative architecture** that was standard in the 1990s-2000s era. It prioritizes simplicity, determinism, and reliability over latency hiding. While this approach works acceptably for space combat games on LAN or low-latency connections, it does not implement modern techniques like client-side prediction, server reconciliation, or lag compensation that are standard in contemporary multiplayer shooters.

The game's latency handling is **reactive rather than predictive**—it measures and adapts to network conditions but does not hide latency from players. This makes XPilot more sensitive to network quality than modern games, but also simpler to understand, debug, and maintain.

---

## References

- `src/server/netserver.c` - Network server implementation
- `src/server/frame.c` - Frame update generation
- `src/server/update.c` - Game state updates
- `src/client/netclient.c` - Client network handling
- `src/common/packet.h` - Packet type definitions
- `src/server/connection.h` - Connection state structures

---

*Document generated from analysis of XPilot NG 4.6.3 source code*
