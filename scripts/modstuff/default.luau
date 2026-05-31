local default = {env_obj = {}, env_obj_joint = {}, player = {}}
local default_player = {}

default_player["tori"] = {}
default_player["tori"]["body"] = {}
default_player["tori"]["body"]["head"] = {}
default_player["tori"]["body"]["head"]["shape"] = Api["SHAPE_SPHERE"]
default_player["tori"]["body"]["head"]["pos"] = "0.00 0.85 2.67"
default_player["tori"]["body"]["head"]["sides"] = "0.20 0.00 0.00"
default_player["tori"]["body"]["head"]["radius"] = "0.20"
default_player["tori"]["body"]["head"]["density"] = "0.25"

default_player["tori"]["body"]["breast"] = {}
default_player["tori"]["body"]["breast"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["breast"]["pos"] = "0.00 0.90 2.20"
default_player["tori"]["body"]["breast"]["sides"] = "0.20 0.10 0.30"
default_player["tori"]["body"]["breast"]["density"] = "0.25"

default_player["tori"]["body"]["chest"] = {}
default_player["tori"]["body"]["chest"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["chest"]["pos"] = "0.00 0.90 1.95"
default_player["tori"]["body"]["chest"]["sides"] = "0.50 0.15 0.20"
default_player["tori"]["body"]["chest"]["density"] = "0.25"

default_player["tori"]["body"]["stomach"] = {}
default_player["tori"]["body"]["stomach"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["stomach"]["pos"] = "0.00 0.95 1.75"
default_player["tori"]["body"]["stomach"]["sides"] = "0.35 0.15 0.20"
default_player["tori"]["body"]["stomach"]["density"] = "0.25"

default_player["tori"]["body"]["groin"] = {}
default_player["tori"]["body"]["groin"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["groin"]["pos"] = "0.00 1.00 1.55"
default_player["tori"]["body"]["groin"]["sides"] = "0.20 0.15 0.20"
default_player["tori"]["body"]["groin"]["density"] = "0.25"

default_player["tori"]["body"]["l_pecs"] = {}
default_player["tori"]["body"]["l_pecs"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["l_pecs"]["pos"] = "0.25 0.90 2.15"
default_player["tori"]["body"]["l_pecs"]["sides"] = "0.20 0.15 0.20"
default_player["tori"]["body"]["l_pecs"]["density"] = "0.25"

default_player["tori"]["body"]["l_biceps"] = {}
default_player["tori"]["body"]["l_biceps"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["l_biceps"]["pos"] = "0.55 0.90 2.30"
default_player["tori"]["body"]["l_biceps"]["sides"] = "0.40 0.20 0.20"
default_player["tori"]["body"]["l_biceps"]["density"] = "0.25"

default_player["tori"]["body"]["l_triceps"] = {}
default_player["tori"]["body"]["l_triceps"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["l_triceps"]["pos"] = "0.95 0.90 2.30"
default_player["tori"]["body"]["l_triceps"]["sides"] = "0.40 0.10 0.10"
default_player["tori"]["body"]["l_triceps"]["density"] = "0.25"

default_player["tori"]["body"]["l_hand"] = {}
default_player["tori"]["body"]["l_hand"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["l_hand"]["pos"] = "1.35 0.85 2.30"
default_player["tori"]["body"]["l_hand"]["sides"] = "0.22 0.22 0.22"
default_player["tori"]["body"]["l_hand"]["density"] = "0.25"

default_player["tori"]["body"]["r_pecs"] = {}
default_player["tori"]["body"]["r_pecs"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["r_pecs"]["pos"] = "-0.25 0.90 2.15"
default_player["tori"]["body"]["r_pecs"]["sides"] = "0.20 0.15 0.20"
default_player["tori"]["body"]["r_pecs"]["density"] = "0.25"

default_player["tori"]["body"]["r_biceps"] = {}
default_player["tori"]["body"]["r_biceps"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["r_biceps"]["pos"] = "-0.55 0.90 2.30"
default_player["tori"]["body"]["r_biceps"]["sides"] = "0.40 0.20 0.20"
default_player["tori"]["body"]["r_biceps"]["density"] = "0.25"

default_player["tori"]["body"]["r_triceps"] = {}
default_player["tori"]["body"]["r_triceps"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["r_triceps"]["pos"] = "-0.95 0.90 2.30"
default_player["tori"]["body"]["r_triceps"]["sides"] = "0.40 0.10 0.10"
default_player["tori"]["body"]["r_triceps"]["density"] = "0.25"

default_player["tori"]["body"]["r_hand"] = {}
default_player["tori"]["body"]["r_hand"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["r_hand"]["pos"] = "-1.35 0.85 2.30"
default_player["tori"]["body"]["r_hand"]["sides"] = "0.22 0.22 0.22"
default_player["tori"]["body"]["r_hand"]["density"] = "0.25"

default_player["tori"]["body"]["l_butt"] = {}
default_player["tori"]["body"]["l_butt"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["l_butt"]["pos"] = "0.20 1.00 1.45"
default_player["tori"]["body"]["l_butt"]["sides"] = "0.10 0.10 0.10"
default_player["tori"]["body"]["l_butt"]["density"] = "0.25"

default_player["tori"]["body"]["l_thigh"] = {}
default_player["tori"]["body"]["l_thigh"]["shape"] = Api["SHAPE_CAPSULE"]
default_player["tori"]["body"]["l_thigh"]["pos"] = "0.20 1.00 1.10"
default_player["tori"]["body"]["l_thigh"]["sides"] = "0.12 0.30 0.20"
default_player["tori"]["body"]["l_thigh"]["radius"] = "0.12"
default_player["tori"]["body"]["l_thigh"]["length"] = "0.30"
default_player["tori"]["body"]["l_thigh"]["density"] = "0.25"

default_player["tori"]["body"]["l_leg"] = {}
default_player["tori"]["body"]["l_leg"]["shape"] = Api["SHAPE_CAPSULE"]
default_player["tori"]["body"]["l_leg"]["pos"] = "0.20 1.00 0.50"
default_player["tori"]["body"]["l_leg"]["sides"] = "0.13 0.30 0.00"
default_player["tori"]["body"]["l_leg"]["radius"] = "0.13"
default_player["tori"]["body"]["l_leg"]["length"] = "0.30"
default_player["tori"]["body"]["l_leg"]["density"] = "0.25"

default_player["tori"]["body"]["l_foot"] = {}
default_player["tori"]["body"]["l_foot"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["l_foot"]["pos"] = "0.20 0.90 0.10"
default_player["tori"]["body"]["l_foot"]["sides"] = "0.20 0.50 0.08"
default_player["tori"]["body"]["l_foot"]["density"] = "0.25"

default_player["tori"]["body"]["r_butt"] = {}
default_player["tori"]["body"]["r_butt"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["r_butt"]["pos"] = "-0.20 1.00 1.45"
default_player["tori"]["body"]["r_butt"]["sides"] = "0.10 0.10 0.10"
default_player["tori"]["body"]["r_butt"]["density"] = "0.25"

default_player["tori"]["body"]["r_thigh"] = {}
default_player["tori"]["body"]["r_thigh"]["shape"] = Api["SHAPE_CAPSULE"]
default_player["tori"]["body"]["r_thigh"]["pos"] = "-0.20 1.00 1.10"
default_player["tori"]["body"]["r_thigh"]["sides"] = "0.12 0.30 0.00"
default_player["tori"]["body"]["r_thigh"]["radius"] = "0.12"
default_player["tori"]["body"]["r_thigh"]["length"] = "0.30"
default_player["tori"]["body"]["r_thigh"]["density"] = "0.25"

default_player["tori"]["body"]["r_leg"] = {}
default_player["tori"]["body"]["r_leg"]["shape"] = Api["SHAPE_CAPSULE"]
default_player["tori"]["body"]["r_leg"]["pos"] = "-0.20 1.00 0.50"
default_player["tori"]["body"]["r_leg"]["sides"] = "0.13 0.30 0.00"
default_player["tori"]["body"]["r_leg"]["radius"] = "0.13"
default_player["tori"]["body"]["r_leg"]["length"] = "0.30"
default_player["tori"]["body"]["r_leg"]["density"] = "0.25"

default_player["tori"]["body"]["r_foot"] = {}
default_player["tori"]["body"]["r_foot"]["shape"] = Api["SHAPE_BOX"]
default_player["tori"]["body"]["r_foot"]["pos"] = "-0.20 0.90 0.10"
default_player["tori"]["body"]["r_foot"]["sides"] = "0.20 0.50 0.08"
default_player["tori"]["body"]["r_foot"]["density"] = "0.25"

default_player["tori"]["joint"] = {}
default_player["tori"]["joint"]["neck"] = {}
default_player["tori"]["joint"]["neck"]["radius"] = "0.11"
default_player["tori"]["joint"]["neck"]["pos"] = "0.00 0.90 2.50"
default_player["tori"]["joint"]["neck"]["strength"] = "1.00"
default_player["tori"]["joint"]["neck"]["velocity"] = "50.00"
default_player["tori"]["joint"]["neck"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["neck"]["range"] = "0.50 -0.50"
default_player["tori"]["joint"]["neck"]["connections"] = "head breast"

default_player["tori"]["joint"]["chest"] = {}
default_player["tori"]["joint"]["chest"]["radius"] = "0.18"
default_player["tori"]["joint"]["chest"]["pos"] = "0.00 0.90 2.05"
default_player["tori"]["joint"]["chest"]["strength"] = "4.00"
default_player["tori"]["joint"]["chest"]["velocity"] = "20.00"
default_player["tori"]["joint"]["chest"]["axis"] = "0.00 0.00 1.00"
default_player["tori"]["joint"]["chest"]["range"] = "1.00 -1.00"
default_player["tori"]["joint"]["chest"]["connections"] = "breast chest"

default_player["tori"]["joint"]["lumbar"] = {}
default_player["tori"]["joint"]["lumbar"]["radius"] = "0.16"
default_player["tori"]["joint"]["lumbar"]["pos"] = "0.00 0.95 1.85"
default_player["tori"]["joint"]["lumbar"]["strength"] = "3.00"
default_player["tori"]["joint"]["lumbar"]["velocity"] = "5.00"
default_player["tori"]["joint"]["lumbar"]["axis"] = "0.00 1.00 0.00"
default_player["tori"]["joint"]["lumbar"]["range"] = "0.50 -0.50"
default_player["tori"]["joint"]["lumbar"]["connections"] = "chest stomach"

default_player["tori"]["joint"]["abs"] = {}
default_player["tori"]["joint"]["abs"]["radius"] = "0.14"
default_player["tori"]["joint"]["abs"]["pos"] = "0.00 1.00 1.65"
default_player["tori"]["joint"]["abs"]["strength"] = "3.00"
default_player["tori"]["joint"]["abs"]["velocity"] = "5.00"
default_player["tori"]["joint"]["abs"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["abs"]["range"] = "0.40 -0.60"
default_player["tori"]["joint"]["abs"]["connections"] = "stomach groin"

default_player["tori"]["joint"]["l_pecs"] = {}
default_player["tori"]["joint"]["l_pecs"]["radius"] = "0.18"
default_player["tori"]["joint"]["l_pecs"]["pos"] = "0.15 0.85 2.20"
default_player["tori"]["joint"]["l_pecs"]["strength"] = "3.00"
default_player["tori"]["joint"]["l_pecs"]["velocity"] = "15.00"
default_player["tori"]["joint"]["l_pecs"]["axis"] = "0.00 0.00 1.00"
default_player["tori"]["joint"]["l_pecs"]["range"] = "1.50 -0.70"
default_player["tori"]["joint"]["l_pecs"]["connections"] = "breast l_pecs"

default_player["tori"]["joint"]["l_shoulder"] = {}
default_player["tori"]["joint"]["l_shoulder"]["radius"] = "0.18"
default_player["tori"]["joint"]["l_shoulder"]["pos"] = "0.35 0.90 2.30"
default_player["tori"]["joint"]["l_shoulder"]["strength"] = "4.00"
default_player["tori"]["joint"]["l_shoulder"]["velocity"] = "20.00"
default_player["tori"]["joint"]["l_shoulder"]["axis"] = "0.00 1.00 0.00"
default_player["tori"]["joint"]["l_shoulder"]["range"] = "1.20 -1.20"
default_player["tori"]["joint"]["l_shoulder"]["connections"] = "l_pecs l_biceps"

default_player["tori"]["joint"]["l_elbow"] = {}
default_player["tori"]["joint"]["l_elbow"]["radius"] = "0.16"
default_player["tori"]["joint"]["l_elbow"]["pos"] = "0.75 0.90 2.30"
default_player["tori"]["joint"]["l_elbow"]["strength"] = "2.00"
default_player["tori"]["joint"]["l_elbow"]["velocity"] = "40.00"
default_player["tori"]["joint"]["l_elbow"]["axis"] = "0.00 0.00 1.00"
default_player["tori"]["joint"]["l_elbow"]["range"] = "2.30 0.05"
default_player["tori"]["joint"]["l_elbow"]["connections"] = "l_biceps l_triceps"

default_player["tori"]["joint"]["l_wrist"] = {}
default_player["tori"]["joint"]["l_wrist"]["radius"] = "0.11"
default_player["tori"]["joint"]["l_wrist"]["pos"] = "1.20 0.90 2.30"
default_player["tori"]["joint"]["l_wrist"]["strength"] = "2.00"
default_player["tori"]["joint"]["l_wrist"]["velocity"] = "60.00"
default_player["tori"]["joint"]["l_wrist"]["axis"] = "0.00 0.00 1.00"
default_player["tori"]["joint"]["l_wrist"]["range"] = "2.00 -0.30"
default_player["tori"]["joint"]["l_wrist"]["connections"] = "l_triceps l_hand"

default_player["tori"]["joint"]["r_pecs"] = {}
default_player["tori"]["joint"]["r_pecs"]["radius"] = "0.18"
default_player["tori"]["joint"]["r_pecs"]["pos"] = "-0.15 0.85 2.20"
default_player["tori"]["joint"]["r_pecs"]["strength"] = "3.00"
default_player["tori"]["joint"]["r_pecs"]["velocity"] = "15.00"
default_player["tori"]["joint"]["r_pecs"]["axis"] = "0.00 0.00 1.00"
default_player["tori"]["joint"]["r_pecs"]["range"] = "1.50 -0.70"
default_player["tori"]["joint"]["r_pecs"]["connections"] = "breast r_pecs"

default_player["tori"]["joint"]["r_shoulder"] = {}
default_player["tori"]["joint"]["r_shoulder"]["radius"] = "0.18"
default_player["tori"]["joint"]["r_shoulder"]["pos"] = "0.35 0.90 2.30"
default_player["tori"]["joint"]["r_shoulder"]["strength"] = "4.00"
default_player["tori"]["joint"]["r_shoulder"]["velocity"] = "20.00"
default_player["tori"]["joint"]["r_shoulder"]["axis"] = "0.00 1.00 0.00"
default_player["tori"]["joint"]["r_shoulder"]["range"] = "1.20 -1.20"
default_player["tori"]["joint"]["r_shoulder"]["connections"] = "r_pecs r_biceps"

default_player["tori"]["joint"]["r_elbow"] = {}
default_player["tori"]["joint"]["r_elbow"]["radius"] = "0.16"
default_player["tori"]["joint"]["r_elbow"]["pos"] = "0.75 0.90 2.30"
default_player["tori"]["joint"]["r_elbow"]["strength"] = "2.00"
default_player["tori"]["joint"]["r_elbow"]["velocity"] = "40.00"
default_player["tori"]["joint"]["r_elbow"]["axis"] = "0.00 0.00 1.00"
default_player["tori"]["joint"]["r_elbow"]["range"] = "2.30 0.05"
default_player["tori"]["joint"]["r_elbow"]["connections"] = "r_biceps r_triceps"

default_player["tori"]["joint"]["r_wrist"] = {}
default_player["tori"]["joint"]["r_wrist"]["radius"] = "0.11"
default_player["tori"]["joint"]["r_wrist"]["pos"] = "1.20 0.90 2.30"
default_player["tori"]["joint"]["r_wrist"]["strength"] = "2.00"
default_player["tori"]["joint"]["r_wrist"]["velocity"] = "60.00"
default_player["tori"]["joint"]["r_wrist"]["axis"] = "0.00 0.00 1.00"
default_player["tori"]["joint"]["r_wrist"]["range"] = "2.00 -0.30"
default_player["tori"]["joint"]["r_wrist"]["connections"] = "r_triceps r_hand"

default_player["tori"]["joint"]["l_glute"] = {}
default_player["tori"]["joint"]["l_glute"]["radius"] = "0.16"
default_player["tori"]["joint"]["l_glute"]["pos"] = "0.10 1.10 1.51"
default_player["tori"]["joint"]["l_glute"]["strength"] = "4.00"
default_player["tori"]["joint"]["l_glute"]["velocity"] = "10.00"
default_player["tori"]["joint"]["l_glute"]["axis"] = "0.00 1.00 0.00"
default_player["tori"]["joint"]["l_glute"]["range"] = "1.20 -0.20"
default_player["tori"]["joint"]["l_glute"]["connections"] = "groin l_butt"

default_player["tori"]["joint"]["l_hip"] = {}
default_player["tori"]["joint"]["l_hip"]["radius"] = "0.16"
default_player["tori"]["joint"]["l_hip"]["pos"] = "0.21 1.00 1.40"
default_player["tori"]["joint"]["l_hip"]["strength"] = "2.00"
default_player["tori"]["joint"]["l_hip"]["velocity"] = "12.00"
default_player["tori"]["joint"]["l_hip"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["l_hip"]["range"] = "1.00 -1.80"
default_player["tori"]["joint"]["l_hip"]["connections"] = "l_butt l_thigh"

default_player["tori"]["joint"]["l_knee"] = {}
default_player["tori"]["joint"]["l_knee"]["radius"] = "0.14"
default_player["tori"]["joint"]["l_knee"]["pos"] = "0.20 1.00 0.80"
default_player["tori"]["joint"]["l_knee"]["strength"] = "4.00"
default_player["tori"]["joint"]["l_knee"]["velocity"] = "40.00"
default_player["tori"]["joint"]["l_knee"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["l_knee"]["range"] = "2.30 0.00"
default_player["tori"]["joint"]["l_knee"]["connections"] = "l_thigh l_leg"

default_player["tori"]["joint"]["l_ankle"] = {}
default_player["tori"]["joint"]["l_ankle"]["radius"] = "0.11"
default_player["tori"]["joint"]["l_ankle"]["pos"] = "0.20 1.05 0.25"
default_player["tori"]["joint"]["l_ankle"]["strength"] = "1.00"
default_player["tori"]["joint"]["l_ankle"]["velocity"] = "8.00"
default_player["tori"]["joint"]["l_ankle"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["l_ankle"]["range"] = "1.10 -0.30"
default_player["tori"]["joint"]["l_ankle"]["connections"] = "l_leg l_foot"

default_player["tori"]["joint"]["r_glute"] = {}
default_player["tori"]["joint"]["r_glute"]["radius"] = "0.16"
default_player["tori"]["joint"]["r_glute"]["pos"] = "-0.10 1.10 1.51"
default_player["tori"]["joint"]["r_glute"]["strength"] = "4.00"
default_player["tori"]["joint"]["r_glute"]["velocity"] = "10.00"
default_player["tori"]["joint"]["r_glute"]["axis"] = "0.00 1.00 0.00"
default_player["tori"]["joint"]["r_glute"]["range"] = "1.20 -0.20"
default_player["tori"]["joint"]["r_glute"]["connections"] = "groin r_butt"

default_player["tori"]["joint"]["r_hip"] = {}
default_player["tori"]["joint"]["r_hip"]["radius"] = "0.16"
default_player["tori"]["joint"]["r_hip"]["pos"] = "-0.21 1.00 1.40"
default_player["tori"]["joint"]["r_hip"]["strength"] = "2.00"
default_player["tori"]["joint"]["r_hip"]["velocity"] = "12.00"
default_player["tori"]["joint"]["r_hip"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["r_hip"]["range"] = "1.00 -1.80"
default_player["tori"]["joint"]["r_hip"]["connections"] = "r_butt r_thigh"

default_player["tori"]["joint"]["r_knee"] = {}
default_player["tori"]["joint"]["r_knee"]["radius"] = "0.14"
default_player["tori"]["joint"]["r_knee"]["pos"] = "-0.20 1.00 0.80"
default_player["tori"]["joint"]["r_knee"]["strength"] = "4.00"
default_player["tori"]["joint"]["r_knee"]["velocity"] = "40.00"
default_player["tori"]["joint"]["r_knee"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["r_knee"]["range"] = "2.30 0.00"
default_player["tori"]["joint"]["r_knee"]["connections"] = "r_thigh r_leg"

default_player["tori"]["joint"]["r_ankle"] = {}
default_player["tori"]["joint"]["r_ankle"]["radius"] = "0.11"
default_player["tori"]["joint"]["r_ankle"]["pos"] = "-0.20 1.05 0.25"
default_player["tori"]["joint"]["r_ankle"]["strength"] = "1.00"
default_player["tori"]["joint"]["r_ankle"]["velocity"] = "8.00"
default_player["tori"]["joint"]["r_ankle"]["axis"] = "1.00 0.00 0.00"
default_player["tori"]["joint"]["r_ankle"]["range"] = "1.10 -0.30"
default_player["tori"]["joint"]["r_ankle"]["connections"] = "r_leg r_foot"

default["player"][1] = default_player["tori"]
default["player"][2] = default_player["tori"]

return default
