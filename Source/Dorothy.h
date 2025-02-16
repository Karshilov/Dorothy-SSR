/* Copyright (c) 2021 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "Const/Header.h"
#include "Basic/AutoreleasePool.h"
#include "Basic/Content.h"
#include "Support/Geometry.h"
#include "Lua/LuaEngine.h"
#include "Lua/LuaHandler.h"
#include "Event/Event.h"
#include "Event/Listener.h"
#include "Event/EventQueue.h"
#include "Basic/Application.h"
#include "Basic/Director.h"
#include "Basic/Scheduler.h"
#include "Common/Async.h"
#include "Support/Array.h"
#include "Support/Dictionary.h"
#include "Support/Common.h"
#include "Input/TouchDispather.h"
#include "Node/Node.h"
#include "Cache/TextureCache.h"
#include "Basic/View.h"
#include "Basic/Camera.h"
#include "Cache/ShaderCache.h"
#include "Basic/Renderer.h"
#include "Basic/VGRender.h"
#include "Effect/Effect.h"
#include "Node/Sprite.h"
#include "Input/TouchDispather.h"
#include "GUI/ImGuiDora.h"
#include "Node/Label.h"
#include "Node/Particle.h"
#include "Cache/ParticleCache.h"
#include "Node/RenderTarget.h"
#include "Node/ClipNode.h"
#include "Node/DrawNode.h"
#include "Node/VGNode.h"
#include "Cache/ClipCache.h"
#include "Cache/FrameCache.h"
#include "Animation/Action.h"
#include "Animation/ModelDef.h"
#include "Node/Model.h"
#include "Cache/AtlasCache.h"
#include "Cache/SkeletonCache.h"
#include "Node/Spine.h"
#include "Cache/ModelCache.h"
#include "Physics/PhysicsWorld.h"
#include "Physics/BodyDef.h"
#include "Physics/Body.h"
#include "Physics/Sensor.h"
#include "Physics/JointDef.h"
#include "Physics/Joint.h"
#include "Cache/SoundCache.h"
#include "Audio/Sound.h"
#include "Node/Menu.h"
#include "Input/Keyboard.h"
#include "Entity/Entity.h"
#include "Cache/SVGCache.h"
#include "Basic/Database.h"
#include "Cache/DragonBoneCache.h"
#include "Node/DragonBone.h"

#include "Platformer/Define.h"
#include "Platformer/AINode.h"
#include "Platformer/AI.h"
#include "Platformer/BulletDef.h"
#include "Platformer/Bullet.h"
#include "Platformer/Data.h"
#include "Platformer/VisualCache.h"
#include "Platformer/Face.h"
#include "Platformer/Unit.h"
#include "Platformer/UnitAction.h"
#include "Platformer/PlatformCamera.h"
#include "Platformer/PlatformWorld.h"

#include "ML/ML.h"
