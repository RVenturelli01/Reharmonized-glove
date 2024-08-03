import threading
import sounddevice as sd
import numpy as np
import cv2
import mediapipe as mp
import matplotlib.pyplot as plt
import time


quattro_zero_r = 1.5
otto_zero_r = 1.5
dodici_zero_r = 1.5
quattro_zero_r_prev = 1.5
otto_zero_r_prev = 1.5
dodici_zero_r_prev = 1.5

angle = 0


class Sound:
    def __init__(self, amplitude, freq):
        self.freq = freq
        self.amplitude = amplitude
        self.starting_point = 0

    def set_freq(self, freq):
        self.freq = freq

    def set_amplitude(self, amplitude):
        self.amplitude = amplitude

    def get_wave(self, n_samples, sample_time):
        t_initial = self.starting_point / (self.freq * 2 * np.pi)
        t_final = t_initial + n_samples * sample_time
        t = np.linspace(t_initial, t_final, n_samples)
        wave = self.amplitude * np.sin(2 * np.pi * self.freq * t)
        self.starting_point = t_final * self.freq * 2 * np.pi
        return wave

class TimerThread():
    def count(self):
        self.flag = False
        time.sleep(5)
        self.flag = True

# timer = TimerThread()  # Set the timer interval to 10 seconds
# timer.count()

# Plot settings________________________________________________________________________________


x_data_1, y_data_1 = [], []
x_data_2, y_data_2 = [], []
x_data_3, y_data_3 = [], []
x_data_4, y_data_4 = [], []
start_time = time.time()
plot_flag = False
def update_plot():
    global x_data_1, y_data_1, x_data_2, y_data_2, x_data_3, y_data_3, x_data_4, y_data_4, start_time
    fig, ((ax_1, ax_2), (ax_3, ax_4)) = plt.subplots(2, 2, figsize=(10, 8))
    line_1, = ax_1.plot(x_data_1, y_data_1)
    line_2, = ax_2.plot(x_data_2, y_data_2)
    line_3, = ax_3.plot(x_data_3, y_data_3)
    line_4, = ax_4.plot(x_data_4, y_data_4)
    ax_1.set_ylim(1, 2)
    ax_2.set_ylim(1, 2)
    ax_3.set_ylim(1, 2)
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
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=2,
                        min_detection_confidence=0.7, min_tracking_confidence=0.4)


circle_color = [[255, 0, 0] for _ in range(21)]

def detectHandsLandmarks(image, hands, display=True):
    global wave, t, otto_zero_r_prev, angle, quattro_zero_r, otto_zero_r, dodici_zero_r, quattro_zero_r_prev, otto_zero_r_prev, dodici_zero_r_prev, plot_flag
    imageRGB = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
    results = hands.process(imageRGB)
    
    if display:
        if results.multi_hand_landmarks:
            plot_flag = True
            for hand_landmarks in results.multi_hand_landmarks:
                for lm_i, lm in enumerate(hand_landmarks.landmark):
                    h, w, c = image.shape
                    cx, cy = int(lm.x * w), int(lm.y * h)
                    cv2.circle(image, (cx, cy), 5, (circle_color[lm_i][0], circle_color[lm_i][1], circle_color[lm_i][2]), cv2.FILLED)
                    if lm_i == 0:
                        zero_x, zero_y, zero_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                    elif lm_i == 2:
                        due_x, due_y, due_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                    elif lm_i == 4:
                        quattro_x, quattro_y, quattro_z = int(lm.x * w), int(lm.y * h), int(lm.z * w)
                        quattro_zero = ((quattro_x - zero_x) ** 2 + (quattro_y - zero_y) ** 2 + (quattro_z - zero_z) ** 2) ** 0.5
                        due_zero = ((due_x - zero_x) ** 2 + (due_y - zero_y) ** 2 + (due_z - zero_z) ** 2) ** 0.5
                        quattro_zero_r = round(quattro_zero / due_zero, 2)
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
                        otto_zero_r = round(otto_zero / cinque_zero, 2)
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
                        dodici_zero_r = round(dodici_zero / nove_zero, 2)
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
        else: plot_flag = False
        cv2.imshow('Hand Tracking', image)
    else:
        return results

wave_lock = threading.Lock()
# Sound settings_______________________________________________________________________________
samplerate = 44100
wave_duration = 10
initial_frequency = 440  # Initial frequency in Hz

wave = Sound(0,1)


def callback(outdata, frames, time, status):
    global wave
    if status:
        print(status)

    with wave_lock:
        wave_ = wave.get_wave(frames, 1/samplerate)
        outdata[:] = wave_.reshape(-1, 1)

grancassa_switch = False
piatto_switch = False
rullante_switch = False
def stream_audio():
    global quattro_zero_r, otto_zero_r, dodici_zero_r, grancassa_switch, piatto_switch, rullante_switch, circle_color
    while True:
        time.sleep(0.1)
        if(quattro_zero_r>1.55 and not(grancassa_switch)):
            #print("grancassa OFF")
            circle_color[4][0] = 255
            circle_color[3][0] = 255
            circle_color[2][0] = 255
            circle_color[4][1] = 0
            circle_color[3][1] = 0
            circle_color[2][1] = 0
            circle_color[4][2] = 0
            circle_color[3][2] = 0
            circle_color[2][2] = 0
            grancassa_switch = True
        if(quattro_zero_r<1.4 and grancassa_switch):
            #print("grancassa ON")
            circle_color[4][0] = 0
            circle_color[3][0] = 0
            circle_color[2][0] = 0
            circle_color[4][1] = 255
            circle_color[3][1] = 255
            circle_color[2][1] = 255
            circle_color[4][2] = 255
            circle_color[3][2] = 255
            circle_color[2][2] = 255
            grancassa_switch = False
        if(otto_zero_r>1.6 and not(piatto_switch)):
            #print("piatto OFF")
            circle_color[8][0] = 255
            circle_color[7][0] = 255
            circle_color[6][0] = 255
            circle_color[8][1] = 0
            circle_color[7][1] = 0
            circle_color[6][1] = 0
            circle_color[8][2] = 0
            circle_color[7][2] = 0
            circle_color[6][2] = 0
            piatto_switch = True
        if(otto_zero_r<1.35 and piatto_switch):
            #print("piatto ON")
            circle_color[8][0] = 0
            circle_color[7][0] = 0
            circle_color[6][0] = 0
            circle_color[8][1] = 255
            circle_color[7][1] = 255
            circle_color[6][1] = 255
            circle_color[8][2] = 255
            circle_color[7][2] = 255
            circle_color[6][2] = 255
            piatto_switch = False
        if(dodici_zero_r>1.70 and not(rullante_switch)):
            #print("rullante OFF")
            circle_color[12][0] = 255
            circle_color[11][0] = 255
            circle_color[10][0] = 255
            circle_color[12][1] = 0
            circle_color[11][1] = 0
            circle_color[10][1] = 0
            circle_color[12][2] = 0
            circle_color[11][2] = 0
            circle_color[10][2] = 0
            rullante_switch = True
        if(dodici_zero_r<1.45 and rullante_switch):
            #print("rullante ON")
            circle_color[12][0] = 0
            circle_color[11][0] = 0
            circle_color[10][0] = 0
            circle_color[12][1] = 255
            circle_color[11][1] = 255
            circle_color[10][1] = 255
            circle_color[12][2] = 255
            circle_color[11][2] = 255
            circle_color[10][2] = 255
            rullante_switch = False
        
    
    # with sd.OutputStream(samplerate=samplerate, channels=1, callback=callback, blocksize=1024):
    #     while True:
    #         time.sleep(1)

def custom_thread():
    global wave
    while cap.isOpened():
        ret, frame = cap.read()
        frame = cv2.flip(frame, 1)
        if not ret:
            break

        detectHandsLandmarks(frame, hands, display=True)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    cap.release()
    cv2.destroyAllWindows()


def main():
    stream_thread = threading.Thread(target=stream_audio, args=())
    stream_thread.daemon = True
    stream_thread.start()

    custom_thread_instance = threading.Thread(target=custom_thread, args=())
    custom_thread_instance.daemon = True
    custom_thread_instance.start()
    
    update_plot()

    while True:
        time.sleep(10)

if __name__ == "__main__":
    main()
