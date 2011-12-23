#include "PhysicsSpringConstraint.h"

#include "Node.h"
#include "PhysicsRigidBody.h"

namespace gameplay
{

PhysicsSpringConstraint::PhysicsSpringConstraint(PhysicsRigidBody* a, PhysicsRigidBody* b)
{
    // Initialize the physics rigid body references since we don't call the PhysicsConstraint constructor that does it properly automatically.
    _a = a;
    _b = b;

    Vector3 origin = centerOfMassMidpoint(a->getNode(), b->getNode());
    _constraint = new btGeneric6DofSpringConstraint(*a->_body, *b->_body, getTransformOffset(a->getNode(), origin), getTransformOffset(b->getNode(), origin), true);
}

PhysicsSpringConstraint::PhysicsSpringConstraint(PhysicsRigidBody* a, const Quaternion& rotationOffsetA, const Vector3& translationOffsetA,
                                                 PhysicsRigidBody* b, const Quaternion& rotationOffsetB, const Vector3& translationOffsetB)
{
    // Initialize the physics rigid body references since we don't call the PhysicsConstraint constructor that does it properly automatically.
    _a = a;
    _b = b;

    // Take scale into account for the translation offsets.
    Vector3 sA;
    a->getNode()->getWorldMatrix().getScale(&sA);
    Vector3 tA(translationOffsetA.x * sA.x, translationOffsetA.y * sA.y, translationOffsetA.z * sA.z);

    Vector3 sB;
    b->getNode()->getWorldMatrix().getScale(&sB);
    Vector3 tB(translationOffsetB.x * sB.x, translationOffsetB.y * sB.y, translationOffsetB.z * sB.z);

    btTransform frameInA(btQuaternion(rotationOffsetA.x, rotationOffsetA.y, rotationOffsetA.z, rotationOffsetA.w), btVector3(tA.x, tA.y, tA.z));
    btTransform frameInB(btQuaternion(rotationOffsetB.x, rotationOffsetB.y, rotationOffsetB.z, rotationOffsetB.w), btVector3(tB.x, tB.y, tB.z));
    _constraint = new btGeneric6DofSpringConstraint(*a->_body, *b->_body, frameInA, frameInB, true);
}

PhysicsSpringConstraint::~PhysicsSpringConstraint()
{
    // Used
}

void PhysicsSpringConstraint::setStrength(SpringProperty property, float strength)
{
    if (strength < MATH_EPSILON)
        ((btGeneric6DofSpringConstraint*)_constraint)->enableSpring(property, false);
    else
    {
        ((btGeneric6DofSpringConstraint*)_constraint)->enableSpring(property, true);
        ((btGeneric6DofSpringConstraint*)_constraint)->setStiffness(property, strength);
        ((btGeneric6DofSpringConstraint*)_constraint)->setEquilibriumPoint(property);
    }
}

void PhysicsSpringConstraint::setDamping(SpringProperty property, float damping)
{
    ((btGeneric6DofSpringConstraint*)_constraint)->setDamping(property, damping);
    ((btGeneric6DofSpringConstraint*)_constraint)->setEquilibriumPoint(property);
}

}
