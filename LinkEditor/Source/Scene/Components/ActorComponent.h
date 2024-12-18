﻿#pragma once
#include "pch.h"

LINK_EDITOR_NAMESPACE_BEGIN

class Actor;

class ActorComponent
{
public:
    ActorComponent();
    virtual ~ActorComponent();

    Actor* GetOwner() const;

    virtual void BeginPlay();
    virtual void EndPlay();
    
public:
    std::vector<std::string> ComponentTags;

private:
    Actor* Owner = nullptr;
};

LINK_EDITOR_NAMESPACE_END
