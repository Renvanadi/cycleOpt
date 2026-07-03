# Bike Motion Optimizer - Requirements Specification

# 1. Project Overview

## Objective

Develop a software system that generates an optimal cycling motion for an individual rider.

The user provides

- Body dimensions
- Bicycle geometry
- Riding conditions
- Riding objective

The system constructs a customized OpenSim musculoskeletal model and solves an optimal control problem using OpenSim Moco.

The output is a full-body optimal cycling motion over one crank revolution together with muscle activations and biomechanical analysis.

Future versions will compare the optimal motion with an actual rider using motion capture.

---

# 2. Use Cases

## UC-1 Generate Optimal Cycling Motion

Input

- Rider body dimensions
- Bicycle geometry
- Riding conditions
- Optimization mode

Process

- Build customized OpenSim model
- Build optimization problem
- Solve using Moco/IPOPT

Output

- Optimal motion
- Muscle activation
- Joint angles
- Metabolic cost
- Analysis graphs

---

## UC-2 Compare with Real Rider

Input

- Video
- Optimal solution

Process

- Pose estimation
- Inverse Kinematics
- Error analysis

Output

- RMSE
- Joint angle comparison
- Muscle usage comparison

---

# 3. System Architecture

Input Module

Responsibilities

- GUI
- Read configuration
- Validate inputs

↓

Model Builder

Responsibilities

- Load generic.osim
- Scale body dimensions
- Generate bicycle geometry
- Build customized OpenSim model

↓

Optimization Solver

Responsibilities

- Construct MocoProblem
- Add objectives
- Add constraints
- Execute IPOPT

↓

Post Processor

Responsibilities

- Read .sto solution
- Compute metrics
- Generate plots

↓

Visualizer

Responsibilities

- Display motion
- Display graphs
- Export results

↓

Motion Comparator (future)

Responsibilities

- Import video
- Pose estimation
- Inverse Kinematics
- Compare with optimal motion

---

# 4. Data Model

## RiderProfile

height

weight

pelvis_width

torso_length

thigh_length

shank_length

foot_length

upper_arm_length

forearm_length

shoulder_width

---

## BikeGeometry

The bicycle geometry shall be represented using actual frame geometry parameters instead of simplified fitting parameters.

Required parameters include, but are not limited to:

Frame geometry

- wheelbase
- chainstay_length
- seat_tube_length
- top_tube_length
- effective_top_tube_length
- down_tube_length
- head_tube_length
- seat_tube_angle
- head_tube_angle
- bottom_bracket_drop
- bottom_bracket_height
- fork_length
- fork_offset (rake)
- front_center
- rear_center
- stack
- reach

Cockpit

- stem_length
- stem_angle
- handlebar_width
- handlebar_reach
- handlebar_drop

Drivetrain

- crank_length
- Q_factor
- pedal_spindle_length

Wheel/Tire

- wheel_radius
- tire_radius

Contact points

- saddle_height
- saddle_setback
- saddle_tilt
- hood_position
- hand_position

The geometry model should be extensible so that additional bicycle parameters can be added without modifying existing code.

---

## RideCondition

target_power

cadence

road_gradient

wind_speed (future)

air_density (future)

optimization_mode

Possible optimization modes

- EnergyMin
- LargeMusclePriority
- UpperBodyUsage
- CustomObjective

---

## OptimizationResult

time

joint_angles

joint_velocities

muscle_activations

muscle_forces

joint_torques

metabolic_cost

power

---

# 5. Optimization Problem

State Variables

- Joint angles
- Joint velocities
- Muscle states

Control Variables

- Muscle excitations

Parameters

- Rider profile
- Bicycle geometry
- Riding condition

Constraints

- Periodicity
- Constant cadence
- Constant power
- Joint range of motion
- Muscle force limits
- Saddle contact
- Handlebar contact
- Pedal contact

Objectives

EnergyMin

Minimize total metabolic energy

MuscleActivationMin

Minimize squared muscle activations

LargeMusclePriority

Prefer recruitment of larger muscles

UpperBodyUsage

Encourage upper-body contribution

The objective system should support adding multiple weighted objectives.

---

# 6. GUI Specification

Left Panel

Rider

Bike

Ride Conditions

Optimization Mode

Run Button

Center

3D Motion Viewer

Right

Muscle Activation

Joint Angles

Power

Metabolic Cost

Comparison Results

---

# 7. File Structure

generic.osim

↓

scaled_model.osim

↓

bike_model.osim

↓

problem_definition

↓

optimal_solution.sto

↓

analysis.csv

↓

report.pdf

---

# 8. Public API

ModelBuilder

buildModel()

Solver

solve()

PostProcessor

analyze()

Visualizer

show()

ResultExporter

exportCSV()

exportPDF()

MotionComparator

analyzeVideo()

compare()

---

# 9. Development Roadmap

Phase 1

Load OpenSim model

Scale rider dimensions

Phase 2

Generate bicycle geometry

Attach rider to bicycle

Phase 3

Create optimization problem

Solve constant-power cycling

Phase 4

Implement multiple objective functions

Phase 5

GUI implementation

Phase 6

Video comparison

Phase 7

Aerodynamic model

Phase 8

Standing pedaling

Phase 9

Bicycle roll dynamics

Phase 10

Fatigue model

---

# 10. Software Requirements

Language

C++20

GUI

Qt6

Optimization

OpenSim Moco

Solver

IPOPT

Build System

CMake

Version Control

Git

Platform

Ubuntu 24.04 LTS

The architecture should be modular and extensible.

Each module should have a single responsibility.

Future optimization objectives, bicycle models, and comparison algorithms should be addable without modifying existing modules.