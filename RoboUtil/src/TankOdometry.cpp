#include "TankOdometry.h"

TankOdometry* TankOdometry::m_instance = nullptr;

TankOdometry * TankOdometry::GetInstance() {
    if(!m_instance) {
        m_instance = new TankOdometry;
    }
    return m_instance;
}

void TankOdometry::Initialize(EncoderConfig leftEncoderConfig, EncoderConfig rightEncoderConfig,
        Pose currentPose) {
    m_leftTicksToDist = (leftEncoderConfig.wheelDiameter * PI) / leftEncoderConfig.ticksPerWheelRevolution;
    m_rightTicksToDist = (rightEncoderConfig.wheelDiameter * PI) / rightEncoderConfig.ticksPerWheelRevolution;
    m_lastLeftEncoderDist = 0;
    m_lastRightEncoderDist = 0;
    SetCurrentPose(currentPose);
}

/**
 * This function is used to update the internal calculated position of the robot
 *
 * To maximize accuracy in robot position estimation, this function should be called as frequently as possible, ideally
 * at a rate of 50 Hz or greater.
 *
 * @param leftEncoderRawTicks The distance in inches measured by the left encoder since when the tracker was last initialized
 * @param rightEncoderRawTicks The distance in inches measured by the left encoder since when the tracker was last initialized
 * @param gyroAngle The current yaw of the robot as measured by the gyro
 */
void TankOdometry::Update(double leftEncoderRawTicks, double rightEncoderRawTicks, Angle gyroAngle) {
    double leftDist = leftEncoderRawTicks * m_leftTicksToDist;
    double rightDist = rightEncoderRawTicks * m_rightTicksToDist;

    if(m_poseReset) {
        m_lastLeftEncoderDist = leftDist;
        m_lastRightEncoderDist = rightDist;
        m_gyroInitialAngle = gyroAngle;
        m_poseReset = false;
    }

    double leftDelta = leftDist - m_lastLeftEncoderDist;
    double rightDelta = rightDist - m_lastRightEncoderDist;
    double distanceMoved = (leftDelta + rightDelta) / 2.0;

    Vector robotTranslation = Vector::FromXY(distanceMoved * gyroAngle.getCos(),
                                             distanceMoved * gyroAngle.getSin());
    m_robotPose.position.transformBy(robotTranslation);
    m_robotPose.angle = gyroAngle.getRotateBy(m_gyroInitialAngle.getInverse());

    m_lastLeftEncoderDist = leftDist;
    m_lastRightEncoderDist = rightDist;
}

/**
 * Set the current pose of the robot
 *
 * @param currentPose Robot pose
 */
void TankOdometry::SetCurrentPose(Pose currentPose) {
    m_robotPose = currentPose;
    m_poseReset = true;
}

/**
 * Reset the positions of the left and right encoders. This function must be called immediately after zeroing encoders.
 *
 * Units are in ticks
 *
 * @param leftEncoderTicks
 * @param rightEncoderTicks
 */
void TankOdometry::ResetEncoderTicks(double leftEncoderTicks, double rightEncoderTicks) {
    m_lastLeftEncoderDist = leftEncoderTicks * m_leftTicksToDist;
    m_lastRightEncoderDist = rightEncoderTicks * m_rightTicksToDist;
}



