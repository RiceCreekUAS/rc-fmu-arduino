// control_mgr.cpp - high level control/autopilot interface
//
// Written by Curtis Olson, started January 2006.
//
// Copyright (C) 2006  Curtis L. Olson  - http://www.flightgear.org/~curt
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Library General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

#include <stdio.h>

#include "../nodes.h"

#include "tecs.h"
#include "fcs_mgr.h"

void fcs_mgr_t::init() {
    // initialize the SBUS receiver
    sbus.init();

    // initialize the pilot interface (RC in, out & mixer)
    inceptors.init();

    // initialize and build the autopilot controller from the property
    // tree config (/config/autopilot)
    ap.init();

    printf("Autopilot initialized\n");
}

// send a reset signal to all ap modules that support it.  This gives each
// component a chance to update it's state to reset for current conditions,
// eliminate transients, etc.
void fcs_mgr_t::reset() {
    // FIXME: events->log("controls", "global reset called");
    ap.reset();
}

void fcs_mgr_t::copy_pilot_inputs() {
    // This function copies the pilot inputs to the flight/engine
    // outputs.  This creates a manual pass through mode.  Consider
    // that manaul pass-through is handled with less latency directly
    // on APM2/BFS/Aura3 hardware if available.

    float aileron = inceptors_node.getDouble("roll");
    control_node.setDouble( "aileron", aileron );

    float elevator = inceptors_node.getDouble("pitch");
    control_node.setDouble( "elevator", elevator );

    float rudder = inceptors_node.getDouble("yaw");
    control_node.setDouble( "rudder", rudder );

    double flaps = inceptors_node.getDouble("flaps");
    control_node.setDouble("flaps", flaps );

    double gear = inceptors_node.getDouble("gear");
    control_node.setDouble("gear", gear );

    double throttle = inceptors_node.getDouble("power");
    control_node.setDouble("throttle", throttle );
}

void fcs_mgr_t::update(float dt) {
    // sanity check
    if ( dt > 1.0 ) { dt = 0.01; }
    if ( dt < 0.00001 ) { dt = 0.01; }

    if ( inceptors.read() ) {
        bool ap_state = inceptors_node.getBool("ap_enabled");
        static bool last_ap_state = ap_state;
        if ( ap_state and !last_ap_state ) {
            printf("ap enabled\n");
        } else if ( !ap_state and last_ap_state ) {
            printf("ap disabled (manaul flight)\n");
        }
        last_ap_state = ap_state;
    }

    // call for a global fcs component reset when activating ap master
    // switch
    static bool last_master_switch = false;
    bool master_switch = inceptors_node.getBool("master_switch");
    if ( master_switch != last_master_switch ) {
        if ( master_switch ) {
            reset();            // reset the ap; transient mitigation
        }
        last_master_switch = master_switch;
    }

    // update tecs (total energy) values and error metrics
    update_tecs();

    // update the autopilot stages (even in manual flight mode.)  This
    // keeps the differential value up to date, tracks manual inputs,
    // and keeps more continuity in the flight when the mode is
    // switched to autopilot.
    ap.update( dt );

    // copy pilot inputs to flight control outputs when not in
    // autopilot mode
    if ( !master_switch ) {
        copy_pilot_inputs();
    }

    inceptors.write(); // fixme: this should become effectors after we move switches to be owned by inceptors
}

// global shared instance
fcs_mgr_t *fcs_mgr = NULL;