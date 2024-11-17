# Simulation parameters
set val(chan)   Channel/WirelessChannel
set val(prop)   Propagation/TwoRayGround
set val(netif)  Phy/WirelessPhy
set val(mac)    Mac/802_11
set val(ifq)    Queue/DropTail/PriQueue
set val(ll)     LL
set val(ant)    Antenna/OmniAntenna
set val(x)      1000  ;# X dimension of the topography
set val(y)      1000  ;# Y dimension of the topography
set val(nn)     20    ;# Number of nodes
set val(stop)   200   ;# Simulation time
set val(mobility_model) "random_waypoint"  ;# Mobility model

# Security parameters
set val(key_size)        256  ;# Key size in bits
set val(update_interval) 10   ;# Pseudonym update interval
set val(min_speed)       5    ;# Minimum node speed
set val(max_speed)       20   ;# Maximum node speed
set val(transmission_range) 250  ;# Radio transmission range
