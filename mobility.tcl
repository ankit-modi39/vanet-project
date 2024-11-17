# Mobility Pattern Generator

# Set random movements for each node
for {set i 0} {$i < 20} {incr i} {
    # Initial position
    $node_($i) set X_ [expr rand()*1000]
    $node_($i) set Y_ [expr rand()*1000]
    $node_($i) set Z_ 0.0
    
    # Schedule multiple movements
    for {set t 0} {$t < 200} {set t [expr $t + 20]} {
        set dest_x [expr rand()*1000]
        set dest_y [expr rand()*1000]
        set speed [expr 5.0 + rand()*15.0]
        $ns at $t "$node_($i) setdest $dest_x $dest_y $speed"
    }
}