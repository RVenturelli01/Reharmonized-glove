import threading
import numpy as np
import cv2
import mediapipe as mp
import matplotlib.pyplot as plt
import time

# calibration settings - real hand distance in cm
due_zero_cm = 7
quattro_zero_max_cm = 5.5 + due_zero_cm
quattro_zero_min_cm = 0.5*quattro_zero_max_cm
cinque_zero_cm = 9
otto_zero_max_cm = 7.5 + cinque_zero_cm
otto_zero_min_cm = 0.4*otto_zero_max_cm
nove_zero_cm = 8.7
dodici_zero_max_cm = 7.7 + nove_zero_cm
dodici_zero_min_cm = 0.3*dodici_zero_max_cm

# 
quattro_zero_r = 1.5
otto_zero_r = 1.5
dodici_zero_r = 1.5
quattro_zero_r_prev = 1.5
otto_zero_r_prev = 1.5
dodici_zero_r_prev = 1.5

angle = 0


# Plot settings________________________________________________________________________________

x_data_1, y_data_1 = [], []
x_data_2, y_data_2 = [], []
x_data_3, y_data_3 = [], []
x_data_4, y_data_4 = [], []
start_time = time.time()
plot_flag = False
def plot():
    global x_data_1, y_data_1, x_data_2, y_data_2, x_data_3, y_data_3, x_data_4, y_data_4, start_time
    fig, ((ax_1, ax_2), (ax_3, ax_4)) = plt.subplots(2, 2, figsize=(10, 8))
    line_1, = ax_1.plot(x_data_1, y_data_1)
    line_2, = ax_2.plot(x_data_2, y_data_2)
    line_3, = ax_3.plot(x_data_3, y_data_3)
    line_4, = ax_4.plot(x_data_4, y_data_4)
    ax_1.set_ylim(-0.5, 1.5)
    ax_2.set_ylim(-0.5, 1.5)
    ax_3.set_ylim(-0.5, 1.5)
    ax_4.set_ylim(-180, 0)

    ax_1.set_title('pollice')
    ax_1.set_xlabel('time (s)')
    ax_1.set_ylabel('(relative) flexing')

    ax_2.set_title('indice')
    ax_2.set_xlabel('time (s)')
    ax_2.set_ylabel('(relative) flexing')

    ax_3.set_title('medio')
    ax_3.set_xlabel('time (s)')
    ax_3.set_ylabel('(relative) flexing')

    ax_4.set_title('orientamento')
    ax_4.set_xlabel('time (s)')
    ax_4.set_ylabel('degrees')
    
    window_size = 50
    plt.tight_layout()
    plt.show(block=False)
    while True:
        if plot_flag:
            line_1.set_xdata(x_data_1[-window_size:])
            line_1.set_ydata(y_data_1[-window_size:])
            line_2.set_xdata(x_data_2[-window_size:])
            line_2.set_ydata(y_data_2[-window_size:])
            line_3.set_xdata(x_data_3[-window_size:])
            line_3.set_ydata(y_data_3[-window_size:])
            line_4.set_xdata(x_data_4[-window_size:])
            line_4.set_ydata(y_data_4[-window_size:])
            
            ax_1.relim()
            ax_1.autoscale_view()
            ax_2.relim()
            ax_2.autoscale_view()
            ax_3.relim()
            ax_3.autoscale_view()
            ax_4.relim()
            ax_4.autoscale_view()

            fig.canvas.draw()
            fig.canvas.flush_events()
        else:
            fig.canvas.flush_events()

# Vision settings_______________________________________________________________________________
cap = cv2.VideoCapture(0)  # 0 indicates the primary camera

mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1,
                        min_detection_confidence=0.5, min_tracking_confidence=0.4)


circle_color = [[255, 0, 0] for _ in range(21)]

tool_1_switch = False
tool_2_switch = False
tool_3_switch = False
def update_color():
    global quattro_zero_r, otto_zero_r, dodici_zero_r,tool_1_switch, tool_2_switch, tool_3_switch, circle_color
    
    if(quattro_zero_r<0.4 and not(tool_1_switch)):
        circle_color[2] = [255, 0, 0]
        circle_color[3] = [255, 0, 0]
        circle_color[4] = [255, 0, 0]
        tool_1_switch = True
    if(quattro_zero_r>0.5 and tool_1_switch):
        circle_color[2] = [0, 255, 255]
        circle_color[3] = [0, 255, 255]
        circle_color[4] = [0, 255, 255]
        tool_1_switch = False
    if(otto_zero_r<0.4 and not(tool_2_switch)):
        circle_color[6] = [255, 0, 0]
        circle_color[7] = [255, 0, 0]
        circle_color[8] = [255, 0, 0]
        tool_2_switch = True
    if(otto_zero_r>0.5 and tool_2_switch):
        circle_color[6] = [0, 255, 255]
        circle_color[7] = [0, 255, 255]
        circle_color[8] = [0, 255, 255]
        tool_2_switch = False
    if(dodici_zero_r<0.4 and not(tool_3_switch)):
        circle_color[10] = [255, 0, 0]
        circle_color[11] = [255, 0, 0]
        circle_color[12] = [255, 0, 0]
        tool_3_switch = True
    if(dodici_zero_r>0.5 and tool_3_switch):
        circle_color[10] = [0, 255, 255]
        circle_color[11] = [0, 255, 255]
        circle_color[12] = [0, 255, 255]
        tool_3_switch = False

def map(x, in_min, in_max, out_min, out_max):
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min


def detectHandsLandmarks(image, hands):
    global wave, t, otto_zero_r_prev, angle, quattro_zero_r, otto_zero_r, dodici_zero_r, quattro_zero_r_prev, otto_zero_r_prev, dodici_zero_r_prev, plot_flag
    imageRGB = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = hands.process(imageRGB)
    
    if results.multi_hand_landmarks:
        plot_flag = True
        for hand_landmarks in results.multi_hand_landmarks:
            for lm_i, lm in enumerate(hand_landmarks.landmark):
                h, w, c = image.shape
                cx, cy = int(lm.x * w), int(lm.y * h)
                update_color()
                cv2.circle(image, (cx, cy), 5, (circle_color[lm_i][0], circle_color[lm_i][1], circle_color[lm_i][2]), cv2.FILLED)
                if lm_i == 0:
                    zero_x, zero_y, zero_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                elif lm_i == 2:
                    due_x, due_y, due_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                elif lm_i == 4:
                    quattro_x, quattro_y, quattro_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                    quattro_zero = ((quattro_x - zero_x) ** 2 + (quattro_y - zero_y) ** 2 + (quattro_z - zero_z) ** 2) ** 0.5
                    due_zero = ((due_x - zero_x) ** 2 + (due_y - zero_y) ** 2 + (due_z - zero_z) ** 2) ** 0.5
                    quattro_zero_cm = quattro_zero*due_zero_cm/due_zero
                    quattro_zero_r = 1 - map(quattro_zero_cm, quattro_zero_min_cm, quattro_zero_max_cm, 0, 1)
                    quattro_zero_r = quattro_zero_r_prev*0.3 + quattro_zero_r*0.7
                    quattro_zero_r_prev = quattro_zero_r
                    current_time = time.time() - start_time
                    x_data_1.append(current_time)
                    y_data_1.append(quattro_zero_r)
                elif lm_i == 5:
                    cinque_x, cinque_y, cinque_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                elif lm_i == 8:
                    otto_x, otto_y, otto_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                    otto_zero = ((otto_x - zero_x) ** 2 + (otto_y - zero_y) ** 2 + (otto_z - zero_z) ** 2) ** 0.5
                    cinque_zero = ((cinque_x - zero_x) ** 2 + (cinque_y - zero_y) ** 2 + (cinque_z - zero_z) ** 2) ** 0.5
                    otto_zero_cm = otto_zero*cinque_zero_cm/cinque_zero
                    otto_zero_r = 1 - map(otto_zero_cm, otto_zero_min_cm,otto_zero_max_cm, 0, 1)
                    otto_zero_r = otto_zero_r_prev*0.3+otto_zero_r*0.7
                    otto_zero_r_prev = otto_zero_r
                    current_time = time.time() - start_time
                    x_data_2.append(current_time)
                    y_data_2.append(otto_zero_r)
                elif lm_i == 9:
                    nove_x, nove_y, nove_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                elif lm_i == 12:
                    dodici_x, dodici_y, dodici_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                    dodici_zero = ((dodici_x - zero_x) ** 2 + (dodici_y - zero_y) ** 2 + (dodici_z - zero_z) ** 2) ** 0.5
                    nove_zero = ((nove_x - zero_x) ** 2 + (nove_y - zero_y) ** 2 + (nove_z - zero_z) ** 2) ** 0.5
                    dodici_zero_cm = dodici_zero*nove_zero_cm/nove_zero
                    dodici_zero_r = 1 - map(dodici_zero_cm, dodici_zero_min_cm, dodici_zero_max_cm, 0, 1)
                    dodici_zero_r = dodici_zero_r_prev*0.3 + dodici_zero_r*0.7
                    dodici_zero_r_prev = dodici_zero_r
                    current_time = time.time() - start_time
                    x_data_3.append(current_time)
                    y_data_3.append(dodici_zero_r)
                elif lm_i == 17:
                    diciassette_x, diciassette_y, diciassette_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                    delta_x = diciassette_x - cinque_x
                    delta_z = diciassette_z - cinque_z
                    angle = (np.arctan2(delta_z, delta_x)/np.pi)*180
                    
                    current_time = time.time() - start_time
                    x_data_4.append(current_time)
                    y_data_4.append(angle)
    else: 
        plot_flag = False

    cv2.imshow('Hand Tracking', image)


def main():
    plot_thread = threading.Thread(target=plot, args=())
    plot_thread.daemon = True
    plot_thread.start()
    
    while cap.isOpened():
        ret, frame = cap.read()
        frame = cv2.flip(frame, 1)
        if not ret:
            break

        detectHandsLandmarks(frame, hands)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()


if __name__ == "__main__":
    main()
