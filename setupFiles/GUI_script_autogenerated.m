% Script for inverse kinematics (IK) 
%
% Author        : Autogenerate
% Created       : 20XX
% Description   :

% Clear the variables, command window, and all windows
clc; clear; close all;

% Set up the type of model
model_config = ModelConfig('PoCaBot spatial');
cable_set_id = 'original';
trajectory_id = 'traj_1';

% Load the SystemKinematics object from the XML
modelObj = model_config.getModel(cable_set_id);

% Setup the inverse kinematics simulator with the SystemKinematics object
disp('Start Setup Simulation');
sim = InverseKinematicsSimulator(modelObj);
trajectory = model_config.getJointTrajectory(trajectory_id);
disp('Finished Setup Simulation');

% Run the kinematics on the desired trajectory
disp('Start Running Simulation');
sim.run(trajectory);
disp('Finished Running Simulation');

% After running the simulator the data can be plotted
% Refer to the simulator classes to see what can be plotted.
disp('Start Plotting Simulation');
%sim.plotJointSpace();
sim.plotCableLengths();
if(isempty(sim))
    warning('No simulator has been generated. Please press run first'); %#ok<WNTAG>
else
    path_string = CASPR_configuration.LoadHomePath();
    % Check if the log folder exists
    if((exist([path_string,'/data'],'dir')~=7)||(exist([path_string,'/data/videos'],'dir')~=7))
        if((exist([path_string,'/data'],'dir')~=7))
            mkdir([path_string,'/data']);
        end
        mkdir([path_string,'/data/videos']);
    end      
    file_name = [path_string,'/data/videos/kinematics_gui_output.avi'];
    [file,path] = uiputfile(file_name,'Save file name');
    sim.plotMovie(model_config.displayRange, model_config.viewAngle, [path,file], sim.timeVector(length(sim.timeVector)), 700, 700);
end
disp('Finished Plotting Simulation');
