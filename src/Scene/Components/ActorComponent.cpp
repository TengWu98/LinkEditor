#include "ActorComponent.h"

MESH_EDITOR_NAMESPACE_BEGIN

void ActorComponent::BeginPlay()
{
}

void ActorComponent::EndPlay()
{
}

Actor* ActorComponent::GetOwner() const
{
    return Owner;
}

MESH_EDITOR_NAMESPACE_END
