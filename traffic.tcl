# Traffic Pattern Generator

proc create_cbr_connection { src dst start stop } {
    global ns
    
    # Create UDP agent
    set udp [new Agent/UDP]
    $ns attach-agent $src $udp
    
    # Create CBR traffic
    set cbr [new Application/Traffic/CBR]
    $cbr set packetSize_ 512
    $cbr set interval_ 0.1
    $cbr attach-agent $udp
    
    # Create null agent
    set null [new Agent/Null]
    $ns attach-agent $dst $null
    
    # Connect agents
    $ns connect $udp $null
    
    # Schedule events
    $ns at $start "$cbr start"
    $ns at $stop "$cbr stop"
}

# Generate random traffic connections
for {set i 0} {$i < 10} {incr i} {
    set src [expr int(rand()*20)]
    set dst [expr int(rand()*20)]
    if {$src != $dst} {
        create_cbr_connection $src $dst [expr rand()*10] 195.0
    }
}