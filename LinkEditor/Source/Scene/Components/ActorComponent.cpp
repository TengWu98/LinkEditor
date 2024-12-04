#include "ActorComponent.h"

LINK_EDITOR_NAMESPACE_BEGIN

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

LINK_EDITOR_NAMESPACE_END
