# Define simulation parameters
set val(chan)   Channel/WirelessChannel
set val(prop)   Propagation/TwoRayGround
set val(netif)  Phy/WirelessPhy
set val(mac)    Mac/802_11
set val(ifq)    Queue/DropTail/PriQueue
set val(ll)     LL
set val(ant)    Antenna/OmniAntenna
set val(ifqlen) 50
set val(nn)     20  ;# number of nodes
set val(rp)     AODV
set val(x)      1000
set val(y)      1000
set val(stop)   200
set val(cp)     "traffic.tcl"
set val(sc)     "mobility.tcl"

# Initialize Global Variables
set ns [new Simulator]
set tracefd [open results/trace.tr w]
set namtrace [open results/nam.tr w]
set routingfile [open results/routing.tr w]

$ns trace-all $tracefd
$ns namtrace-all-wireless $namtrace $val(x) $val(y)

# Set up topography
set topo [new Topography]
$topo load_flatgrid $val(x) $val(y)

# Create God
create-god $val(nn)

# Configure nodes
$ns node-config -adhocRouting $val(rp) \
                -llType $val(ll) \
                -macType $val(mac) \
                -ifqType $val(ifq) \
                -ifqLen $val(ifqlen) \
                -antType $val(ant) \
                -propType $val(prop) \
                -phyType $val(netif) \
                -channel [new $val(chan)] \
                -topoInstance $topo \
                -agentTrace ON \
                -routerTrace ON \
                -macTrace ON \
                -movementTrace ON

# Create nodes and initialize their positions
for {set i 0} {$i < $val(nn)} {incr i} {
    set node_($i) [$ns node]
    $node_($i) random-motion 0

    # Set initial position
    set x_pos [expr rand()*$val(x)]
    set y_pos [expr rand()*$val(y)]
    $node_($i) set X_ $x_pos
    $node_($i) set Y_ $y_pos
    $node_($i) set Z_ 0.0

    # Create node visualization for NAM
    $ns initial_node_pos $node_($i) 30
}

# Define node movement model
proc set_random_motion {} {
    global ns val node_
    set current_time [$ns now]
    for {set i 0} {$i < $val(nn)} {incr i} {
        set dest_x [expr rand() * $val(x)]
        set dest_y [expr rand() * $val(y)]
        set speed [expr 5.0 + rand() * 15.0]  ;# Speed between 5-20 m/s
        
        # Ensure motion events are scheduled after the current time
        set event_time [expr $current_time + rand() * ($val(stop) - $current_time)]
        $ns at $event_time "$node_($i) setdest $dest_x $dest_y $speed"
    }
    
    # Reschedule set_random_motion in the future
    set next_schedule_time [expr $current_time + 10.0] ;# Reschedule every 10s
    if {$next_schedule_time < $val(stop)} {
        $ns at $next_schedule_time "set_random_motion"
    }
}


# Define traffic pattern
proc create_traffic {} {
    global ns val node_
    
    # Create UDP agents and CBR traffic
    for {set i 0} {$i < [expr $val(nn)-1]} {incr i} {
        # Create UDP agent
        set udp_($i) [new Agent/UDP]
        $ns attach-agent $node_($i) $udp_($i)
        
        # Create CBR traffic over UDP
        set cbr_($i) [new Application/Traffic/CBR]
        $cbr_($i) set packetSize_ 512
        $cbr_($i) set interval_ 0.1
        $cbr_($i) attach-agent $udp_($i)
        
        # Create null agent (traffic sink)
        set null_($i) [new Agent/Null]
        $ns attach-agent $node_([expr ($i+1)%$val(nn)]) $null_($i)
        
        # Connect agents
        $ns connect $udp_($i) $null_($i)
        
        # Start and stop traffic
        $ns at [expr rand()*10.0] "$cbr_($i) start"
        $ns at [expr $val(stop)-5] "$cbr_($i) stop"
    }
}

# Define finish procedure
proc finish {} {
    global ns tracefd namtrace routingfile
    $ns flush-trace
    close $tracefd
    close $namtrace
    close $routingfile
    exec nam results/nam.tr &
    exit 0
}

# Initialize attribute-based authentication
proc init_auth {} {
    global ns val node_
    for {set i 0} {$i < $val(nn)} {incr i} {
        # Assign attributes to nodes (simulated)
        set attributes "vehicle,authorized,trusted"
        puts "Node $i initialized with attributes: $attributes"
    }
}

# Define secure message transmission
proc send_secure_message {src dst msg} {
    global ns
    puts "Secure message from node $src to node $dst: $msg"
    # Here we would implement the actual secure message transmission
    # using our C++ implementation
}

# Schedule events
$ns at 0.0 "init_auth"
$ns at 0.1 "create_traffic"
$ns at 0.1 "set_random_motion"

# Schedule some secure message transmissions
for {set i 0} {$i < 10} {incr i} {
    set src [expr int(rand()*$val(nn))]
    set dst [expr int(rand()*$val(nn))]
    if {$src != $dst} {
        $ns at [expr rand()*$val(stop)] "send_secure_message $src $dst \"Test message $i\""
    }
}

# End simulation
$ns at $val(stop) "finish"

# Start simulation
puts "Starting Simulation..."
$ns run