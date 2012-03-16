#ifndef PHYSICSMOTIONSTATE_H_
#define PHYSICSMOTIONSTATE_H_

#include "Node.h"
#include "PhysicsRigidBody.h"

namespace gameplay
{

/**
 * Interface between GamePlay and Bullet to keep object transforms synchronized properly.
 * 
 * @see btMotionState
 */
class PhysicsMotionState : public btMotionState
{
    friend class PhysicsRigidBody;
    friend class PhysicsCharacter;
    friend class PhysicsConstraint;
    friend class PhysicsGhostObject;

protected:

    /**
     * Creates a physics motion state for a rigid body.
     * 
     * @param node The node that owns the rigid body that the motion state is being created for.
     * @param centerOfMassOffset The translation offset to the center of mass of the rigid body.
     */
    PhysicsMotionState(Node* node, const Vector3* centerOfMassOffset = NULL);

    /**
     * Destructor.
     */
    virtual ~PhysicsMotionState();

    /**
     * @see btMotionState#getWorldTransform
     */
    virtual void getWorldTransform(btTransform &transform) const;

    /**
     * @see btMotionState#setWorldTransform
     */
    virtual void setWorldTransform(const btTransform &transform);

private:

    // Updates the motion state's world transform from the GamePlay Node object's world transform.
    void updateTransformFromNode() const;

    Node* _node;
    btTransform _centerOfMassOffset;
    mutable btTransform _worldTransform;
};

}

#endif
