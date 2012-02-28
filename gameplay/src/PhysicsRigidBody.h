#ifndef PHYSICSRIGIDBODY_H_
#define PHYSICSRIGIDBODY_H_

#include "Mesh.h"
#include "PhysicsConstraint.h"
#include "Transform.h"
#include "Vector3.h"

namespace gameplay
{

class Node;
class PhysicsConstraint;

/**
 * Defines a class for physics rigid bodies.
 */
class PhysicsRigidBody : public Transform::Listener
{
    friend class Node;
    friend class PhysicsCharacter;
    friend class PhysicsConstraint;
    friend class PhysicsController;
    friend class PhysicsFixedConstraint;
    friend class PhysicsGenericConstraint;
    friend class PhysicsHingeConstraint;
    friend class PhysicsSocketConstraint;
    friend class PhysicsSpringConstraint;

public:

    /**
     * Represents the different types of rigid bodies.
     */
    enum Type
    {
        SHAPE_BOX,
        SHAPE_SPHERE,
        SHAPE_NONE,
        SHAPE_MAX = 10
    };

    /** 
     * Defines a pair of rigid bodies that collided (or may collide).
     */
    class CollisionPair
    {
    public:

        /**
         * Constructor.
         */
        CollisionPair(PhysicsRigidBody* rigidBodyA, PhysicsRigidBody* rigidBodyB);

        /**
         * Less than operator (needed for use as a key in map).
         * 
         * @param collisionPair The collision pair to compare.
         * @return True if this pair is "less than" the given pair; false otherwise.
         */
        bool operator<(const CollisionPair& collisionPair) const;

        /** The first rigid body in the collision. */
        PhysicsRigidBody* rigidBodyA;

        /** The second rigid body in the collision. */
        PhysicsRigidBody* rigidBodyB;
    };

    /**
     * Collision listener interface.
     */
    class Listener
    {
        friend class PhysicsRigidBody;
        friend class PhysicsController;

    public:
        /**
         * The type of collision event.
         */
        enum EventType
        {
            /**
             * Event fired when the two rigid bodies start colliding.
             */
            COLLIDING,

            /**
             * Event fired when the two rigid bodies no longer collide.
             */
            NOT_COLLIDING
        };

        /**
         * Destructor.
         */
        virtual ~Listener();

        /**
         * Handles when a collision starts or stops occurring for the rigid body where this listener is registered.
         * 
         * @param type The type of collision event.
         * @param collisionPair The two rigid bodies involved in the collision.
         * @param contactPointA The contact point with the first rigid body (in world space).
         * @param contactPointB The contact point with the second rigid body (in world space).
         */
        virtual void collisionEvent(EventType type, const CollisionPair& collisionPair, const Vector3& contactPointA = Vector3(), const Vector3& contactPointB = Vector3()) = 0;
    };

    /**
     * Adds a collision listener for this rigid body.
     * 
     * @param listener The listener to add.
     * @param body Specifies that only collisions with the given rigid body should trigger a notification.
     */
    void addCollisionListener(Listener* listener, PhysicsRigidBody* body = NULL);

    /**
     * Applies the given force to the rigid body (optionally, from the given relative position).
     * 
     * @param force The force to be applied.
     * @param relativePosition The relative position from which to apply the force.
     */
    void applyForce(const Vector3& force, const Vector3* relativePosition = NULL);

    /**
     * Applies the given force impulse to the rigid body (optionally, from the given relative position).
     * 
     * @param impulse The force impulse to be applied.
     * @param relativePosition The relative position from which to apply the force.
     */
    void applyImpulse(const Vector3& impulse, const Vector3* relativePosition = NULL);

    /**
     * Applies the given torque to the rigid body.
     * 
     * @param torque The torque to be applied.
     */
    void applyTorque(const Vector3& torque);

    /**
     * Applies the given torque impulse to the rigid body.
     * 
     * @param torque The torque impulse to be applied.
     */
    void applyTorqueImpulse(const Vector3& torque);
    
    /**
     * Checks if this rigid body collides with the given rigid body.
     * 
     * @param body The rigid body to test collision with.
     * @return True if this rigid body collides with the given rigid body; false otherwise.
     */
    bool collidesWith(PhysicsRigidBody* body);

    /**
     * Gets the rigid body's angular damping.
     * 
     * @return The angular damping.
     */
    inline float getAngularDamping() const;

    /**
     * Gets the rigid body's angular velocity.
     * 
     * @return The angular velocity.
     */
    inline const Vector3& getAngularVelocity() const;

    /**
     * Gets the rigid body's anisotropic friction.
     * 
     * @return The anisotropic friction.
     */
    inline const Vector3& getAnisotropicFriction() const;

    /**
     * Gets the rigid body's friction.
     * 
     * @return The friction.
     */
    inline float getFriction() const;

    /**
     * Gets the gravity that affects the rigid body (this can
     * be different from the global gravity; @see #setGravity).
     * 
     * @return The gravity.
     */
    inline const Vector3& getGravity() const;

    /**
     * Gets the height at the given point (only for rigid bodies of type HEIGHTFIELD).
     * 
     * @param x The x position.
     * @param y The y position.
     * @return The height at the given point.
     */
    float getHeight(float x, float y) const;

    /**
     * Gets the rigid body's linear damping.
     * 
     * @return The linear damping.
     */
    inline float getLinearDamping() const;

    /**
     * Gets the rigid body's linear velocity.
     * 
     * @return The linear velocity.
     */
    inline const Vector3& getLinearVelocity() const;

    /**
     * Gets the node that the rigid body is attached to.
     * 
     * @return The node.
     */
    inline Node* getNode();

    /**
     * Gets the rigid body's restitution.
     * 
     * @return The restitution.
     */
    inline float getRestitution() const;

    /**
     * Gets whether the rigid body is a kinematic rigid body or not.
     * 
     * @return Whether the rigid body is kinematic or not.
     */
    inline bool isKinematic() const;

    /**
     * Gets whether the rigid body is a static rigid body or not.
     *
     * @return Whether the rigid body is static.
     */
    inline bool isStatic() const;

    /**
     * Gets whether the rigid body is a dynamic rigid body or not.
     *
     * @return Whether the rigid body is dynamic.
     */
    inline bool isDynamic() const;

    /**
     * Sets the rigid body's angular velocity.
     * 
     * @param velocity The angular velocity.
     */
    inline void setAngularVelocity(const Vector3& velocity);

    /**
     * Sets the rigid body's anisotropic friction.
     * 
     * @param friction The anisotropic friction.
     */
    inline void setAnisotropicFriction(const Vector3& friction);

    /**
     * Sets the rigid body's linear and angular damping.
     * 
     * @param linearDamping The linear damping; between 0.0 (minimum) and 1.0 (maximum).
     * @param angularDamping The angular damping; between 0.0 (minimum) and 1.0 (maximum).
     */
    inline void setDamping(float linearDamping, float angularDamping);

    /**
     * Sets the rigid body's friction.
     * 
     * @param friction The friction.
     */
    inline void setFriction(float friction);

    /**
     * Sets the rigid body's gravity (this overrides the global gravity for this rigid body).
     * 
     * @param gravity The gravity.
     */
    inline void setGravity(const Vector3& gravity);

    /**
     * Sets whether the rigid body is a kinematic rigid body or not.
     * 
     * @param kinematic Whether the rigid body is kinematic or not.
     */
    inline void setKinematic(bool kinematic);

    /**
     * Sets the rigid body's linear velocity.
     * 
     * @param velocity The linear velocity.
     */
    inline void setLinearVelocity(const Vector3& velocity);

    /**
     * Sets the rigid body's restitution (or bounciness).
     * 
     * @param restitution The restitution.
     */
    inline void setRestitution(float restitution);

private:

    /**
     * Creates a rigid body.
     * 
     * @param node The node to create a rigid body for; note that the node must have
     *      a model attached to it prior to creating a rigid body for it.
     * @param type The type of rigid body to set.
     * @param mass The mass of the rigid body, in kilograms.
     * @param friction The friction of the rigid body (non-zero values give best simulation results).
     * @param restitution The restitution of the rigid body (this controls the bounciness of
     *      the rigid body; use zero for best simulation results).
     * @param linearDamping The percentage of linear velocity lost per second (between 0.0 and 1.0).
     * @param angularDamping The percentage of angular velocity lost per second (between 0.0 and 1.0).
     */
    PhysicsRigidBody(Node* node, PhysicsRigidBody::Type type, float mass, float friction = 0.5,
        float restitution = 0.0, float linearDamping = 0.0, float angularDamping = 0.0);

    /**
     * Creates a heightfield rigid body.
     * 
     * @param node The node to create the heightfield rigid body for; note that the node must have
     *      a model attached to it prior to creating a rigid body for it.
     * @param image The heightfield image.
     * @param mass The mass of the rigid body, in kilograms.
     * @param friction The friction of the rigid body (non-zero values give best simulation results).
     * @param restitution The restitution of the rigid body (this controls the bounciness of
     *      the rigid body; use zero for best simulation results).
     * @param linearDamping The percentage of linear velocity lost per second (between 0.0 and 1.0).
     * @param angularDamping The percentage of angular velocity lost per second (between 0.0 and 1.0).
     */
    PhysicsRigidBody(Node* node, Image* image, float mass, float friction = 0.5,
        float restitution = 0.0, float linearDamping = 0.0, float angularDamping = 0.0);

    /**
     * Creates a capsule rigid body.
     * 
     * @param node The node to create the heightfield rigid body for; note that the node must have
     *      a model attached to it prior to creating a rigid body for it.
     * @param radius The radius of the capsule.
     * @param height The height of the cylindrical part of the capsule (not including the ends).
     * @param mass The mass of the rigid body, in kilograms.
     * @param friction The friction of the rigid body (non-zero values give best simulation results).
     * @param restitution The restitution of the rigid body (this controls the bounciness of
     *      the rigid body; use zero for best simulation results).
     * @param linearDamping The percentage of linear velocity lost per second (between 0.0 and 1.0).
     * @param angularDamping The percentage of angular velocity lost per second (between 0.0 and 1.0).
     */
    PhysicsRigidBody(Node* node, float radius, float height, float mass, float friction = 0.5,
        float restitution = 0.0, float linearDamping = 0.0, float angularDamping = 0.0);

    /**
     * Destructor.
     */
    ~PhysicsRigidBody();

    /**
     * Private copy constructor to disallow copying.
     */
    PhysicsRigidBody(const PhysicsRigidBody& body);

    /**
     * Creates a rigid body from the rigid body file at the given path.
     * 
     * @param node The node to create a rigid body for; note that the node must have
     *      a model attached to it prior to creating a rigid body for it.
     * @param filePath The path to the rigid body file.
     * @return The rigid body or <code>NULL</code> if the rigid body could not be loaded.
     */
    static PhysicsRigidBody* create(Node* node, const char* filePath);

    /**
     * Creates a rigid body from the specified properties object.
     * 
     * @param node The node to create a rigid body for; note that the node must have
     *      a model attached to it prior to creating a rigid body for it.
     * @param properties The properties object defining the rigid body (must have namespace equal to 'rigidbody').
     * @return The newly created rigid body, or <code>NULL</code> if the rigid body failed to load.
     */
    static PhysicsRigidBody* create(Node* node, Properties* properties);

    // Creates the underlying Bullet Physics rigid body object
    // for a PhysicsRigidBody object using the given parameters.
    static btRigidBody* createRigidBodyInternal(btCollisionShape* shape, float mass, Node* node,
                                                float friction, float restitution, float linearDamping, float angularDamping,
                                                const Vector3* centerOfMassOffset = NULL);

    // Adds a constraint to this rigid body.
    void addConstraint(PhysicsConstraint* constraint);

    // Removes a constraint from this rigid body (used by the constraint destructor).
    void removeConstraint(PhysicsConstraint* constraint);
    
    // Whether or not the rigid body supports constraints fully.
    bool supportsConstraints();

    // Used for implementing getHeight() when the heightfield has a transform that can change.
    void transformChanged(Transform* transform, long cookie);

    // Internal class used to implement the collidesWith(PhysicsRigidBody*) function.
    struct CollidesWithCallback : public btCollisionWorld::ContactResultCallback
    {
        btScalar addSingleResult(btManifoldPoint& cp, 
                                 const btCollisionObject* a, int partIdA, int indexA, 
                                 const btCollisionObject* b, int partIdB, int indexB);

        bool result;
    };

    btCollisionShape* _shape;
    btRigidBody* _body;
    Node* _node;
    std::vector<PhysicsConstraint*> _constraints;
    std::vector<Listener*>* _listeners;
    mutable Vector3* _angularVelocity;
    mutable Vector3* _anisotropicFriction;
    mutable Vector3* _gravity;
    mutable Vector3* _linearVelocity;
    float* _vertexData;
    std::vector<unsigned char*> _indexData;
    float* _heightfieldData;
    unsigned int _width;
    unsigned int _height;
    mutable Matrix* _inverse;
    mutable bool _inverseIsDirty;
};

}

#include "PhysicsRigidBody.inl"

#endif
