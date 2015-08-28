set(BOX2D_VERSION 2.3.2)
include_directories(${BOX2D_INCLUDE_DIR}/..)

set(BOX2D_Collision_SRCS
	Collision/b2BroadPhase.cpp
	Collision/b2CollideCircle.cpp
	Collision/b2CollideEdge.cpp
	Collision/b2CollidePolygon.cpp
	Collision/b2Collision.cpp
	Collision/b2Distance.cpp
	Collision/b2DynamicTree.cpp
	Collision/b2TimeOfImpact.cpp
)
set(BOX2D_Shapes_SRCS
	Collision/Shapes/b2CircleShape.cpp
	Collision/Shapes/b2EdgeShape.cpp
	Collision/Shapes/b2ChainShape.cpp
	Collision/Shapes/b2PolygonShape.cpp
)
set(BOX2D_Common_SRCS
	Common/b2BlockAllocator.cpp
	Common/b2Draw.cpp
	Common/b2Math.cpp
	Common/b2Settings.cpp
	Common/b2StackAllocator.cpp
	Common/b2Timer.cpp
)
set(BOX2D_Dynamics_SRCS
	Dynamics/b2Body.cpp
	Dynamics/b2ContactManager.cpp
	Dynamics/b2Fixture.cpp
	Dynamics/b2Island.cpp
	Dynamics/b2World.cpp
	Dynamics/b2WorldCallbacks.cpp
)
set(BOX2D_Contacts_SRCS
	Dynamics/Contacts/b2CircleContact.cpp
	Dynamics/Contacts/b2Contact.cpp
	Dynamics/Contacts/b2ContactSolver.cpp
	Dynamics/Contacts/b2PolygonAndCircleContact.cpp
	Dynamics/Contacts/b2EdgeAndCircleContact.cpp
	Dynamics/Contacts/b2EdgeAndPolygonContact.cpp
	Dynamics/Contacts/b2ChainAndCircleContact.cpp
	Dynamics/Contacts/b2ChainAndPolygonContact.cpp
	Dynamics/Contacts/b2PolygonContact.cpp
)
set(BOX2D_Joints_SRCS
	Dynamics/Joints/b2DistanceJoint.cpp
	Dynamics/Joints/b2FrictionJoint.cpp
	Dynamics/Joints/b2GearJoint.cpp
	Dynamics/Joints/b2Joint.cpp
	Dynamics/Joints/b2MotorJoint.cpp
	Dynamics/Joints/b2MouseJoint.cpp
	Dynamics/Joints/b2PrismaticJoint.cpp
	Dynamics/Joints/b2PulleyJoint.cpp
	Dynamics/Joints/b2RevoluteJoint.cpp
	Dynamics/Joints/b2RopeJoint.cpp
	Dynamics/Joints/b2WeldJoint.cpp
	Dynamics/Joints/b2WheelJoint.cpp
)
set(BOX2D_Rope_SRCS
	Rope/b2Rope.cpp
)

# Add library
add_library(box2d
	${BOX2D_Joints_SRCS}
	${BOX2D_Contacts_SRCS}
	${BOX2D_Dynamics_SRCS}
	${BOX2D_Common_SRCS}
	${BOX2D_Shapes_SRCS}
	${BOX2D_Collision_SRCS}
	${BOX2D_Rope_SRCS}
)
set_target_properties(box2d PROPERTIES
	CLEAN_DIRECT_OUTPUT 1
	VERSION ${BOX2D_VERSION}
)

# EEV interfacing
set(BOX2D_LIBRARIES box2d)

