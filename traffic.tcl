# Traffic Pattern Generator with Randomized Packets Sent

# Function to create randomized CBR connections
proc create_random_cbr_traffic {} {
    global ns val node_
    
    # Define parameters for randomization
    set total_nodes $val(nn)               ;# Total number of nodes
    set max_connections [expr $total_nodes / 2] ;# Maximum number of CBR flows
    set min_interval 0.1                   ;# Minimum interval between packets
    set max_interval 0.5                   ;# Maximum interval between packets
    set max_start_time 10                  ;# Max start time for CBR traffic
    set max_duration [expr $val(stop) - 10];# Maximum duration of traffic

    # Randomly determine the number of connections
    set num_connections [expr int(rand()*$max_connections) + 1]
    puts "Creating $num_connections CBR connections."

    # Generate randomized CBR connections
    for {set i 0} {$i < $num_connections} {incr i} {
        # Randomly pick source and destination nodes
        set src [expr int(rand()*$total_nodes)]
        set dst [expr int(rand()*$total_nodes)]
        
        # Ensure source and destination are not the same
        while {$src == $dst} {
            set dst [expr int(rand()*$total_nodes)]
        }

        # Randomize the start time and interval
        set start_time [expr rand()*$max_start_time]
        set interval [expr $min_interval + rand()*($max_interval - $min_interval)]
        set duration [expr $start_time + rand()*$max_duration]
        if {$duration > $val(stop)} {
            set duration $val(stop)
        }

        # Create the CBR connection
        puts "Creating CBR: $src -> $dst | Start: $start_time | Interval: $interval | Stop: $duration"
        create_cbr_connection $src $dst $start_time $duration $interval
    }
}

# Function to create a single CBR connection
proc create_cbr_connection {src dst start stop interval} {
    global ns node_

    # Create UDP agent
    set udp [new Agent/UDP]
    $ns attach-agent $node_($src) $udp

    # Create CBR application
    set cbr [new Application/Traffic/CBR]
    $cbr set packetSize_ 512
    $cbr set interval_ $interval
    $cbr attach-agent $udp

    # Create NULL agent
    set null [new Agent/Null]
    $ns attach-agent $node_($dst) $null

    # Connect UDP to NULL
    $ns connect $udp $null

    # Schedule traffic
    $ns at $start "$cbr start"
    $ns at $stop "$cbr stop"
}

# Generate Random Traffic
create_random_cbr_traffic
