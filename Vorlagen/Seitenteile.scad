cube_side_length = 120;
nose_length = 60;
case_thickness = 4;
pcb_length = 100;
hole_radius = 3;

explode = 0; //15;

pcb_length = 100;
pcb_thickness = 1;
pcb_grid = 2.54;

parts_laser_offset = cube_side_length + 10;


union() {
//intersection() {
	cube_sides();

	cube_bottom();

	cube_top();

//	pcb();
}


/*
projection()
	laser_cut_parts();
*/

module laser_cut_parts() {
	translate([0, 0, cube_side_length/2+explode-case_thickness/2])
		cube_side();
	translate([parts_laser_offset, 0, cube_side_length/2+explode-case_thickness/2])
		cube_side();
	translate([parts_laser_offset*2, 0, cube_side_length/2+explode-case_thickness/2])
		cube_side();
	translate([parts_laser_offset*0, parts_laser_offset, cube_side_length/2+explode-case_thickness/2])
		cube_side();
	translate([parts_laser_offset*1, parts_laser_offset, -cube_side_length/2-explode+case_thickness/2])
		cube_top();
	translate([parts_laser_offset*2, parts_laser_offset, cube_side_length/2+explode-case_thickness/2])
		cube_bottom();
}


module cube_sides() {
	for(i = [0:90:270]) {
		rotate([i,90,0])
			cube_side();
	}
}

module cube_side() {
	translate([-cube_side_length/2,-cube_side_length/2,-cube_side_length/2-explode])			difference() {
		part();

		nose_x();
/*		translate([(cube_side_length-nose_length), 0, 0])
			nose_x();
*/
		translate([nose_length, cube_side_length- case_thickness, 0])
			nose_x();


		translate([case_thickness, nose_length, 0])
			nose_y();


		translate([cube_side_length, 0, 0])
			nose_y();
/*		translate([cube_side_length, cube_side_length-nose_length, 0])
			nose_y();*/

	}
}

module cube_bottom() {
	translate([-cube_side_length/2,-cube_side_length/2,-cube_side_length/2-explode])
	
	difference() {
		part();

/*		translate([nose_length, 0, 0])
			nose_x();*/
		translate([0, 0, 0])
			nose_x();

		translate([nose_length, cube_side_length-case_thickness, 0])
			nose_x();

		translate([case_thickness, nose_length, 0])
			nose_y();
	
/*		translate([cube_side_length, nose_length, 0])
			nose_y();*/
		translate([cube_side_length, 0, 0])
			nose_y();
	}
}

module cube_top() {
	translate([-cube_side_length/2,-cube_side_length/2,cube_side_length/2-case_thickness+explode])

	difference() {
		part();

/*		nose_x();*/
		translate([(cube_side_length-nose_length), 0, 0])
			nose_x();

		translate([0, cube_side_length- case_thickness, 0])
			nose_x();
/*		translate([cube_side_length-nose_length, cube_side_length- case_thickness, 0])
			nose_x();*/

		translate([case_thickness, 0, 0])
			nose_y();

/*		translate([case_thickness, cube_side_length-nose_length, 0])
			nose_y();*/

/*		translate([cube_side_length, 0, 0])
			nose_y();*/

		translate([cube_side_length, cube_side_length-nose_length, 0])
			nose_y();
	}
}

module nose_x() {
	cube([nose_length, case_thickness, case_thickness]);
}

module nose_y() {
	translate([-case_thickness,0,0])
		cube([case_thickness, nose_length, case_thickness]);
}

module part() {
	difference() {
		cube([cube_side_length, cube_side_length, case_thickness]);

		for(x=[29.52:2.54*24:90.48]) {
			for(y=[19.36:2.54*16:100.64]) {
				translate([x, y, 0])
					hole();
			}
		}
		translate([cube_side_length/2, cube_side_length/2, 0])
			hole();
	}
}

module hole() {
	translate([0,0,case_thickness/2])
		cylinder(h=case_thickness*2, r=hole_radius, center=true, $fns=20);
}

module pcb() {
	translate([-pcb_length/2,-pcb_length/2,-pcb_length/2-explode])
	difference() {

		cube([pcb_length, pcb_length, pcb_thickness]);

		for(x=[1:39]) {
			for(y=[1:39]) {
				translate([x*pcb_grid-0.8, y*pcb_grid-0.8, 0])
					translate([0,0,case_thickness/2])
						cylinder(h=case_thickness*2, r=1, center=true, $fns=3);
			}
		}
		translate([cube_side_length/4*2, cube_side_length/6*3, 0])
			hole();
	}
}