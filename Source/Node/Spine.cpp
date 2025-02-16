/* Copyright (c) 2021 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Const/Header.h"
#include "Node/Spine.h"
#include "Basic/Content.h"
#include "Cache/AtlasCache.h"
#include "Cache/SkeletonCache.h"
#include "Cache/TextureCache.h"
#include "Basic/Scheduler.h"
#include "Node/Sprite.h"
#include "Node/DrawNode.h"
#include "Effect/Effect.h"
#include "Support/Common.h"
#include "Basic/Director.h"

NS_DOROTHY_BEGIN

class SpineExtension : public spine::DefaultSpineExtension
{
public:
	virtual ~SpineExtension() {}

	virtual char* _readFile(const spine::String& path, int* length)
	{
		int64_t size = 0;
		auto data = SharedContent.loadUnsafe({path.buffer(),path.length()}, size);
		*length = s_cast<int>(size);
		return r_cast<char*>(data);
	}
};

Spine::SpineListener::SpineListener(Spine* owner):_owner(owner)
{ }

void Spine::SpineListener::callback(spine::AnimationState* state, spine::EventType type, spine::TrackEntry* entry, spine::Event* event)
{
	spine::String empty;
	const spine::String& name = (entry && entry->getAnimation()) ? entry->getAnimation()->getName() : empty;
	Slice animationName{name.buffer(), name.length()};
	switch (type)
	{
		case spine::EventType_End:
			if (_owner->_currentAnimationName == animationName)
			{
				_owner->_currentAnimationName.clear();
			}
			break;
		case spine::EventType_Event:
			_owner->emit(animationName, s_cast<Playable*>(_owner));
			break;
		case spine::EventType_Complete:
			_owner->emit("AnimationEnd"_slice, animationName, s_cast<Playable*>(_owner));
			_owner->_lastCompletedAnimationName = animationName;
			break;
		case spine::EventType_Interrupt:
			_owner->_lastCompletedAnimationName.clear();
			break;
		case spine::EventType_Start:
		case spine::EventType_Dispose:
			break;
	}
}

Spine::Spine(String spineStr):
_skeletonData(SharedSkeletonCache.load(spineStr)),
_effect(SharedSpriteRenderer.getDefaultEffect()),
_listener(this)
{ }

Spine::Spine(String skelFile, String atlasFile):
_skeletonData(SharedSkeletonCache.load(skelFile, atlasFile)),
_effect(SharedSpriteRenderer.getDefaultEffect()),
_listener(this)
{ }

bool Spine::init()
{
	if (!Node::init()) return false;
	if (_skeletonData)
	{
		_animationStateData = New<spine::AnimationStateData>(_skeletonData->getSkel());
		_animationState = New<spine::AnimationState>(_animationStateData.get());
		_skeleton = New<spine::Skeleton>(_skeletonData->getSkel());
		auto& slots = _skeleton->getSlots();
		for (size_t i = 0; i < slots.size(); i++)
		{
			spine::Slot* slot = slots[i];
			if (!slot->getBone().isActive()) continue;
			spine::Attachment* attachment = slot->getAttachment();
			if (attachment &&
				!attachment->getRTTI().instanceOf(spine::BoundingBoxAttachment::rtti))
			{
				_bounds = New<spine::SkeletonBounds>();
				setHitTestEnabled(true);
				break;
			}
		}
		this->scheduleUpdate();
		return true;
	}
	return false;
}

void Spine::setSpeed(float var)
{
	_animationState->setTimeScale(var);
	Playable::setSpeed(var);
}

void Spine::setRecovery(float var)
{
	_animationStateData->setDefaultMix(var);
	Playable::setRecovery(var);
}

void Spine::setDepthWrite(bool var)
{
	_flags.set(Spine::DepthWrite, var);
}

bool Spine::isDepthWrite() const
{
	return _flags.isOn(Spine::DepthWrite);
}

void Spine::setHitTestEnabled(bool var)
{
	_flags.set(Spine::HitTest, var);
}

bool Spine::isHitTestEnabled() const
{
	return _flags.isOn(Spine::HitTest);
}

void Spine::setShowDebug(bool var)
{
	if (var)
	{
		if (!_debugLine)
		{
			_debugLine = Line::create();
			addChild(_debugLine);
		}
	}
	else
	{
		if (_debugLine)
		{
			_debugLine->removeFromParent();
			_debugLine = nullptr;
		}
	}
}

bool Spine::isShowDebug() const
{
	return _debugLine != nullptr;
}

void Spine::setLook(String name)
{
	if (name.empty())
	{
		_skeleton->setSkin(nullptr);
		_skeleton->setSlotsToSetupPose();
		Playable::setLook(name);
	}
	else
	{
		Slice skinName = Slice::Empty, skinStr = name;
		auto tokens = name.split(":"_slice);
		if (tokens.size() == 2)
		{
			skinName = tokens.front();
			skinStr = tokens.back();
		}
		tokens = skinStr.split(";"_slice);
		if (!skinName.empty() || tokens.size() > 1)
		{
			if (skinName.empty())
			{
				skinName = "unnamed"_slice;
			}
			_newSkin = New<spine::Skin>(spine::String{skinName.begin(), skinName.size(), false});
			for (const auto& token : tokens)
			{
				auto skin = _skeletonData->getSkel()->findSkin(spine::String{token.begin(), token.size(), false});
				if (skin)
				{
					_newSkin->addSkin(skin);
				}
			}
			_skeleton->setSkin(_newSkin.get());
			_skeleton->setSlotsToSetupPose();
			Playable::setLook(skinName);
		}
		else
		{
			auto skin = _skeletonData->getSkel()->findSkin(spine::String{name.begin(), name.size(), false});
			if (skin)
			{
				_skeleton->setSkin(skin);
				_skeleton->setSlotsToSetupPose();
				Playable::setLook(name);
			}
		}
	}
}

void Spine::setFliped(bool var)
{
	_skeleton->setScaleX(var ? -1.0f : 1.0f);
	Playable::setFliped(var);
}

const std::string& Spine::getCurrent() const
{
	return _currentAnimationName;
}

const std::string& Spine::getLastCompleted() const
{
	return _lastCompletedAnimationName;
}

Vec2 Spine::getKeyPoint(String name) const
{
	auto tokens = name.split("/"_slice);
	if (tokens.size() == 1)
	{
		auto slotName = spine::String{name.begin(),name.size(),false};
		auto slotIndex = _skeleton->findSlotIndex(slotName);
		if (slotIndex < 0) return Vec2::zero;
		if (auto skin = _skeleton->getSkin())
		{
			spine::Vector<spine::Attachment*> attachments;
			skin->findAttachmentsForSlot(slotIndex, attachments);
			for (size_t i = 0; i < attachments.size(); ++i)
			{
				auto attachment = attachments[i];
				if (attachment->getRTTI().isExactly(spine::PointAttachment::rtti))
				{
					spine::PointAttachment* point = s_cast<spine::PointAttachment*>(attachment);
					Vec2 res = Vec2::zero;
					auto& bone = _skeleton->getSlots()[slotIndex]->getBone();
					point->computeWorldPosition(bone, res.x, res.y);
					return res;
				}
			}
		}
		else if (tokens.size() == 2)
		{
			auto slotName = spine::String{tokens.front().begin(), tokens.front().size(), false};
			int slotIndex = _skeleton->findSlotIndex(slotName);
			if (slotIndex < 0) return Vec2::zero;
			auto attachmentName = spine::String{tokens.back().begin(),tokens.back().size(),false};
			auto attachment = _skeleton->getAttachment(slotIndex, attachmentName);
			if (attachment->getRTTI().isExactly(spine::PointAttachment::rtti))
			{
				spine::PointAttachment* point = s_cast<spine::PointAttachment*>(attachment);
				Vec2 res = Vec2::zero;
				auto& bone = _skeleton->getSlots()[slotIndex]->getBone();
				point->computeWorldPosition(bone, res.x, res.y);
				return res;
			}
		}
	}
	return Vec2::zero;
}

float Spine::play(String name, bool loop)
{
	auto animation = _skeletonData->getSkel()->findAnimation(spine::String{name.begin(), name.size(), false});
	if (!animation)
	{
		return 0.0f;
	}
	_currentAnimationName = name;
	float recoveryTime = _animationStateData->getDefaultMix();
	if (recoveryTime > 0.0f)
	{
		_animationState->setEmptyAnimation(0, recoveryTime);
		auto trackEntry = _animationState->addAnimation(0, animation, loop, FLT_EPSILON);
		trackEntry->setListener(&_listener);
		return trackEntry->getAnimationEnd() / std::max(_animationState->getTimeScale(), FLT_EPSILON);
	}
	else
	{
		auto trackEntry = _animationState->setAnimation(0, animation, loop);
		trackEntry->setListener(&_listener);
		return trackEntry->getAnimationEnd() / std::max(_animationState->getTimeScale(), FLT_EPSILON);
	}
}

void Spine::stop()
{
	_animationState->clearTrack(0);
}

std::string Spine::containsPoint(float x, float y)
{
	if (!_bounds || !isHitTestEnabled()) return Slice::Empty;
	if (_bounds->aabbcontainsPoint(x, y))
	{
		if (auto attachment = _bounds->containsPoint(x, y))
		{
			return std::string(
				attachment->getName().buffer(),
				attachment->getName().length());
		}
	}
	return Slice::Empty;
}

std::string Spine::intersectsSegment(float x1, float y1, float x2, float y2)
{
	if (!_bounds || !isHitTestEnabled()) return Slice::Empty;
	if (_bounds->aabbintersectsSegment(x1, y1, x2, y2))
	{
		if (auto attachment = _bounds->intersectsSegment(x1, y1, x2, y2))
		{
			return std::string(
				attachment->getName().buffer(),
				attachment->getName().length());
		}
	}
	return Slice::Empty;
}

void Spine::visit()
{
	_animationState->update(s_cast<float>(getScheduler()->getDeltaTime()));
	_animationState->apply(*_skeleton);
	_skeleton->updateWorldTransform();
	if (_bounds && isHitTestEnabled())
	{
		_bounds->update(*_skeleton, true);
	}
	Node::visit();
}

void Spine::render()
{
	Matrix transform;
	bx::mtxMul(transform, _world, SharedDirector.getViewProjection());
	SharedRendererManager.setCurrent(SharedSpriteRenderer.getTarget());

	if (isShowDebug())
	{
		_debugLine->clear();
	}

	std::vector<SpriteVertex> vertices;
	for (size_t i = 0, n = _skeleton->getSlots().size(); i < n; ++i)
	{
		spine::Slot* slot = _skeleton->getDrawOrder()[i];
		spine::Attachment* attachment = slot->getAttachment();
		if (!attachment) continue;

		BlendFunc blendFunc = BlendFunc::Default;
		switch (slot->getData().getBlendMode())
		{
			case spine::BlendMode_Normal:
				blendFunc = {BlendFunc::SrcAlpha, BlendFunc::InvSrcAlpha};
				break;
			case spine::BlendMode_Additive:
				blendFunc = {BlendFunc::SrcAlpha, BlendFunc::One};
				break;
			case spine::BlendMode_Multiply:
				blendFunc = {BlendFunc::DstColor, BlendFunc::InvSrcAlpha};
				break;
			case spine::BlendMode_Screen:
				blendFunc = {BlendFunc::One, BlendFunc::InvSrcColor};
				break;
			default:
				break;
		}

		uint64_t renderState = (
			BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A |
			BGFX_STATE_MSAA | blendFunc.toValue());
		if (_flags.isOn(Spine::DepthWrite))
		{
			renderState |= (BGFX_STATE_WRITE_Z | BGFX_STATE_DEPTH_TEST_LESS);
		}

		spine::Color skeletonColor = _skeleton->getColor();
		spine::Color slotColor = slot->getColor();
		uint32_t abgr = Color(Vec4{
			skeletonColor.r * slotColor.r,
			skeletonColor.g * slotColor.g,
			skeletonColor.b * slotColor.b,
			skeletonColor.a * slotColor.a * _realColor.getOpacity()}).toABGR();

		Texture2D* texture = nullptr;
		if (attachment->getRTTI().isExactly(spine::RegionAttachment::rtti))
		{
			spine::RegionAttachment* regionAttachment = s_cast<spine::RegionAttachment*>(attachment);
			texture = r_cast<Texture2D*>(r_cast<spine::AtlasRegion*>(regionAttachment->getRendererObject())->page->getRendererObject());
			vertices.assign(4, {0, 0, 0, 1});
			regionAttachment->computeWorldVertices(slot->getBone(), &vertices.front().x, 0, sizeof(SpriteVertex) / sizeof(float));

			for (size_t j = 0, l = 0; j < 4; j++, l+=2)
			{
				SpriteVertex& vertex = vertices[j];
				SpriteVertex oldVert = vertex;
				bx::vec4MulMtx(&vertex.x, &oldVert.x, transform);
				vertex.abgr = abgr;
				vertex.u = regionAttachment->getUVs()[l];
				vertex.v = regionAttachment->getUVs()[l + 1];
			}

			SharedSpriteRenderer.push(
				vertices.data(), vertices.size(),
				_effect, texture, renderState);
		}
		else if (attachment->getRTTI().isExactly(spine::MeshAttachment::rtti))
		{
			spine::MeshAttachment* mesh = s_cast<spine::MeshAttachment*>(attachment);
			texture = r_cast<Texture2D*>(r_cast<spine::AtlasRegion*>(mesh->getRendererObject())->page->getRendererObject());
			size_t verticeLength = mesh->getWorldVerticesLength();
			size_t numVertices = verticeLength / 2;
			vertices.assign(numVertices, {0, 0, 0, 1});
			mesh->computeWorldVertices(*slot, 0, verticeLength, &vertices.front().x, 0, sizeof(SpriteVertex) / sizeof(float));

			for (size_t j = 0, l = 0; j < numVertices; j++, l+=2)
			{
				SpriteVertex& vertex = vertices[j];
				SpriteVertex oldVert = vertex;
				bx::vec4MulMtx(&vertex.x, &oldVert.x, transform);
				vertex.abgr = abgr;
				vertex.u = mesh->getUVs()[l];
				vertex.v = mesh->getUVs()[l + 1];
			}

			auto& meshIndices = mesh->getTriangles();
			SharedSpriteRenderer.push(
				vertices.data(), vertices.size(),
				meshIndices.buffer(), meshIndices.size(),
				_effect, texture, renderState);
		}
		else if (attachment->getRTTI().isExactly(spine::BoundingBoxAttachment::rtti))
		{
			if (isShowDebug() && isHitTestEnabled())
			{
				spine::BoundingBoxAttachment* boundingBox = s_cast<spine::BoundingBoxAttachment*>(attachment);
				auto polygon = _bounds->getPolygon(boundingBox);
				int vertSize = polygon->_count / 2;
				std::vector<Vec2> vertices(vertSize + 1);
				for (int i = 0; i < vertSize; i++)
				{
					float x = polygon->_vertices[i * 2];
					float y = polygon->_vertices[i * 2 + 1];
					vertices[i] = {x, y};
				}
				vertices[vertSize] = vertices[0];
				_debugLine->add(vertices, Color(0xff00ffff));
			}
		}
		vertices.clear();
	}
}

NS_DOROTHY_END

spine::SpineExtension* spine::getDefaultExtension()
{
	return new Dorothy::SpineExtension();
}
