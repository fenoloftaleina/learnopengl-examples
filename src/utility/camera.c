#include "camera.h"

void update_camera_vectors(struct cam_desc* camera) {
    // Calculate the new Front vector
    hmm_vec3 front;
    front.X = cosf(HMM_ToRadians(camera->yaw)) * cosf(HMM_ToRadians(camera->pitch));
    front.Y = sinf(HMM_ToRadians(camera->pitch));
    front.Z = sinf(HMM_ToRadians(camera->yaw)) * cosf(HMM_ToRadians(camera->pitch));
    camera->front = HMM_NormalizeVec3(front);
    // Also re-calculate the Right and Up vector
    // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    camera->right = HMM_NormalizeVec3(HMM_Cross(camera->front, camera->world_up));  
    camera->up    = HMM_NormalizeVec3(HMM_Cross(camera->right, camera->front));
}

struct cam_desc create_camera(hmm_vec3 position, hmm_vec3 up, float yaw, float pitch) {
    struct cam_desc camera;
    camera.position = position;
    camera.world_up = up;
    camera.yaw = yaw;
    camera.pitch = pitch;

    camera.movement_speed = SPEED;
    camera.mouse_sensitivity = SENSITIVITY;
    camera.zoom = ZOOM;

    update_camera_vectors(&camera);
    
    return camera;
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
hmm_mat4 get_view_matrix(const struct cam_desc* camera) {
    hmm_vec3 direction = HMM_AddVec3(camera->position, camera->front);
    return HMM_LookAt(camera->position, direction, camera->up);
}

 // Processes input received from any keyboard-like input system.
 // Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
void process_keyboard(struct cam_desc* camera, enum camera_movement direction, float delta_time) {
    float velocity = camera->movement_speed * delta_time;
    if (direction == CAM_MOV_FORWARD) {
        hmm_vec3 offset = HMM_MultiplyVec3f(camera->front, velocity);
        camera->position = HMM_AddVec3(camera->position, offset);
    }
    else if (direction == CAM_MOV_BACKWARD) {
        hmm_vec3 offset = HMM_MultiplyVec3f(camera->front, velocity);
        camera->position = HMM_SubtractVec3(camera->position, offset);
    }
    else if (direction == CAM_MOV_LEFT) {
        hmm_vec3 offset = HMM_MultiplyVec3f(camera->right, velocity);
        camera->position = HMM_SubtractVec3(camera->position, offset);
    }
    else if (direction == CAM_MOV_RIGHT) {
        hmm_vec3 offset = HMM_MultiplyVec3f(camera->right, velocity);
        camera->position = HMM_AddVec3(camera->position, offset);
    }
}

// Processes input received from a mouse input system. Expects the offset value in both the x and y direction.
void process_mouse_movement(struct cam_desc* camera, float xoffset, float yoffset) {
    xoffset *= camera->mouse_sensitivity;
    yoffset *= camera->mouse_sensitivity;

    camera->yaw   += xoffset;
    camera->pitch += yoffset;

    // Make sure that when pitch is out of bounds, screen doesn't get flipped
    if (camera->pitch > 89.0f) {
        camera->pitch = 89.0f;
    }
    else if (camera->pitch < -89.0f) {
        camera->pitch = -89.0f;
    }

    // Update Front, Right and Up Vectors using the updated Euler angles
    update_camera_vectors(camera);
}

// Processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void process_mouse_scroll(struct cam_desc* camera, float yoffset) {
    if (camera->zoom >= 1.0f && camera->zoom <= 45.0f)
        camera->zoom -= yoffset;
    if (camera->zoom <= 1.0f)
        camera->zoom = 1.0f;
    else if (camera->zoom >= 45.0f)
        camera->zoom = 45.0f;
}
