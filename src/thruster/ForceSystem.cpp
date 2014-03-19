/*
 * ForceSystem.cpp
 *
 *  Created on: Aug 10, 2010
 *      Author: tbabb
 */

#include "ForceSystem.h"
#include "Craft.h"

ForceSystem::ForceSystem() {}

ForceSystem::~ForceSystem() {}

void Forces::applyForce(Vec3d loc, Vec3d f) {
    this->force_inertial  += f;
    this->torque_inertial += loc.cross(f);
}

void Forces::applyForceBodySpace(Vec3d loc, Vec3d f) {
    this->force_body  += f;
    this->torque_body += loc.cross(f); 
}

void Forces::applyTorque(Vec3d loc, Vec3d t) {
    this->torque_inertial += t; //not verified physics!!
}

void Forces::applyTorqueBodySpace(Vec3d loc, Vec3d t) {
    this->torque_body += t; //not verified physics!!
}

Forces Forces::operator+(const Forces& fs) {
    Forces f;
    f.force_body      = this->force_body + fs.force_body;
    f.force_inertial  = this->force_inertial + fs.force_inertial;
    f.torque_body     = this->torque_body + fs.torque_body;
    f.torque_inertial = this->torque_inertial + fs.torque_inertial;
    return f;
}

Forces& Forces::operator+=(const Forces& fs) {
    this->force_body      += fs.force_body;
    this->force_inertial  += fs.force_inertial;
    this->torque_body     += fs.torque_body;
    this->torque_inertial += fs.torque_inertial;
    return *this;
}
