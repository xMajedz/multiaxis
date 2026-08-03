#include "Game.h"

#include <fstream>
#include <sstream>

static GameMod* parsemod(std::istream& data)
{
    int version = 0;
    int context = 0;

    int env_obj_id = 0;
    int env_obj_joint_id = 0;

    int p_id = 0;
    int b_id = 0;
    int j_id = 0;

    std::string b_name;
    std::string j_name;

    std::string line;

    GameMod* mod = new GameMod;
    
    env_obj* current_object = nullptr;
    env_obj_joint* current_object_joint = nullptr;
    
    while (std::getline(data, line)) {
	 std::stringstream datastream(line);
	 std::string dataname;
	    		
	 datastream >> dataname;
		    
	 if (dataname == "version") {
	     datastream >> version;
             continue;
	 } else if (dataname == "gamerule") {
	     continue;
	 } else if (dataname == "env_obj") {
	     context = 1;
				
	     if (datastream >> env_obj_id) {
	         std::string name = "object_" + std::to_string(env_obj_id);
		 current_object = &mod->objects.emplace_back();
		 mod->o_map[name] = mod->objects.size();
	     }
				
	     continue;
	 } else if (dataname == "env_obj_joint") {
	     context = 3;
             
	     if (datastream >> env_obj_joint_id) {
	         current_object_joint = &mod->joints.emplace_back();
	     }
	     			
	     continue;
	  } else if (dataname == "player") {
	      context = 3;
              /*
	      if (datastream >> player_id) {
		  std::string name = "player_" + player_id;

		  if (player_count < Api::rules.numplayers) {
		      b_count = 0;
		      j_count = 0;

		      Player player(player_count, name.data());
		      Api::players_vector.push_back(player);
		      current_player = &Api::players_vector[player_count];

		      player_count += 1;
		   }
	      }
	      */		
	      continue;
	  } else if (dataname == "body") {
	      context = 4;
              /*
	      if (datastream >> body_name) {
		  Body body;
		  body.id_ = b_count;
				body.name_ = body_name;
			    
	            Api::b_map[body_name] = b_count;
				current_player->body.push_back(body);
				current_body = &current_player->body[b_count];

				b_count += 1;
			}
	      */
	       continue;
	   } else if (dataname == "joint") {
	       context = 5;
               /*
	       if (datastream >> joint_name) {
			    Joint joint;
				joint.id_ = j_count;
				joint.name_ = joint_name;
			    
	            current_player->joint.push_back(joint);
	            current_joint = &current_player->joint[j_count];

				j_count += 1;
			}
	       */		
	       continue;
	    }

	    switch(context)
	    {
	    case 0:
		 if (dataname == "turnframes") {
		     datastream >> mod->rules.turnframes;
		 } else if (dataname == "engagedistance") {
		     datastream >> mod->rules.engagedistance;
		 } else if (dataname == "engageheight") {
		     datastream >> mod->rules.engageheight;
		 } else if (dataname == "gravity") {
		     datastream >> mod->rules.gravity.x;
		     datastream >> mod->rules.gravity.y;
		     datastream >> mod->rules.gravity.z;
		 } else if (dataname == "numplayers") {
		     datastream >> mod->rules.numplayers;
		 }
			    
		 break;
	     case 1:
		  if (dataname == "shape") {
		      if (datastream >> dataname) {
			  if (dataname == "box")
			      current_object->shape = 0;
			  else if (dataname == "sphere")
			      current_object->shape = 1;
			  else if (dataname == "capsule")
			      current_object->shape = 2;
			  else if (dataname == "cylinder")
			      current_object->shape = 3;
		      }
		  } else if (dataname == "pos") {
		      datastream >> current_object->position.x;
		      datastream >> current_object->position.y;
		      datastream >> current_object->position.z;
		 } else if (dataname == "mass") {
		      //datastream >> current_object->mass;
		 } else if (dataname == "density") {
		      //datastream >> current_object->density;
		 } else if (dataname == "color") {
		      //float r, g, b, a;
		      datastream >> current_object->color[0];
		      datastream >> current_object->color[1];
		      datastream >> current_object->color[2];
		      datastream >> current_object->color[3];
		 } else if (dataname == "rot") {
		      //Vector3 rot;
		      //datastream >> rot.x;
		      //datastream >> rot.y;
		      //datastream >> rot.z;
		      //Quaternion q = QuaternionFromMatrix(MatrixRotateXYZ(rot));
		      //current_object->orientation.x = q.x;
		      //current_object->orientation.y = q.y;
		      //current_object->orientation.z = q.z;
		      //current_object->orientation.w = q.w;
		  } else if (dataname == "sides") {
		       datastream >> current_object->sides.x;
		       datastream >> current_object->sides.y;
		       datastream >> current_object->sides.z;
		  } else if (dataname == "radius") {
		    //datastream >> current_object->radius;
		  } else if (dataname == "length") {
		    //datastream >> current_object->length;
		  } else if (dataname == "force") {
		      //datastream >> Api::o->force.x;
		      //datastream >> Api::o->force.y;
		      //datastream >> Api::o->force.z;
		  } else if (dataname == "flag") {
		      datastream >> current_object->flag;

		      //current_object->static_ = current_object->flag_ & 1;
		      //current_object->composite_ = current_object->flag_ & 2;
		      //current_object->interactive_ = current_object->flag_ & 4;
		  } else if (dataname == "bounce") {
		    //datastream >> current_object->bounce;
		  } else if (dataname == "friction") {
		    //datastream >> current_object->friction;
		  }	
		  break;
	     }
	}
    
    return mod;
}

static void parsemodstring(std::string content)
{
    std::stringstream s(content);

    parsemod(s);
}

static GameMod* parsemodfile(std::string filename)
{
    std::ifstream file(filename);

    if (!file) return nullptr;

    return parsemod(file);
}

Game::Game(Api& ApiInstance)
  : ApiInstance_(ApiInstance)
  , physics_(nullptr)
  , running_(true)
{
    ApiInstance_.SetGame(this);
}

Game::~Game()
{
    delete physics_;
}

void Game::Update()
{
    ApiInstance_.Update();
    
    if (physics_) physics_->Step(frame_);
}

FrameData Game::GetFrameData()
{
    return frame_;
}

void Game::Quit()
{
    running_ = false;
}

void Game::NewGame()
{
    delete physics_;

    GameMod* mod = parsemodfile("mods/box.tbm");

    physics_ = new GamePhysics(ApiInstance_, frame_, mod);
}

bool Game::ShouldQuit()
{
    return !running_;
}

GamePhysics::GamePhysics(Api& ApiInstance, FrameData& frame, GameMod* mod)
  : ApiInstance_(ApiInstance)
  , frame_(frame)
  , mod_(mod)
{
    b3WorldDef worldDef = b3DefaultWorldDef();
    worldDef.gravity = mod->rules.gravity;

    worldId = b3CreateWorld(&worldDef);

    for (auto o : mod_->objects) {
       auto& o_transform = frame_.transforms.emplace_back();

       o_transform.position = o.position;
       o_transform.sides = o.sides;
       o_transform.color = o.color;

       if (o.flag & 1) {
           /* static */
           b3BodyDef groundDef = b3DefaultBodyDef();
           groundDef.position = o.position;

           o_transform.id = b3CreateBody(worldId, &groundDef);

           b3BoxHull groundBox = b3MakeBoxHull(o.sides.x, o.sides.y, o.sides.z);

           b3ShapeDef groundShapeDef = b3DefaultShapeDef();
           b3CreateHullShape(o_transform.id, &groundShapeDef, &groundBox.base);
       } else {
	    /* dynamic */
            b3BodyDef bodyDef = b3DefaultBodyDef();
            bodyDef.type = b3_dynamicBody;
            bodyDef.position = o.position;

	    o_transform.id = b3CreateBody(worldId, &bodyDef);

	    b3BoxHull dynamicBox = b3MakeBoxHull(o.sides.x, o.sides.y, o.sides.z);

            b3ShapeDef shapeDef = b3DefaultShapeDef();
            shapeDef.density = 1.0f;
            shapeDef.baseMaterial.friction = 0.3f;

	    b3CreateHullShape(o_transform.id, &shapeDef, &dynamicBox.base);
        }
    }
}

GamePhysics::~GamePhysics()
{
    delete mod_;
    b3DestroyWorld(worldId);
}

void GamePhysics::Step(FrameData& frame)
{
    b3World_Step(worldId, (1.0f / 60.0f), 4);

    //   frame.ground_transform.sides = ground_sides;
 
    for (auto& o : frame_.transforms) {
        if (o.flag & 1) {

	} else {
           o.position = b3Body_GetPosition(o.id);
           o.rotation = b3Body_GetRotation(o.id);
        }
    }
}
