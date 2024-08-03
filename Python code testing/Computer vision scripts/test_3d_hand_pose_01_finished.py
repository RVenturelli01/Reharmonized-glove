import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import cv2
import mediapipe as mp

def plot_world_landmarks(ax, hand_landmarks):

    ax.cla()
    ax.set_xlim3d(0, 1)
    ax.set_ylim3d(0, 1)
    ax.set_zlim3d(0, 1)
    ax.set_xlabel('X')
    ax.set_ylabel('Y')
    ax.set_zlabel('Z')


    connections = mp_hands.HAND_CONNECTIONS
    for connection in connections:
        x_values = [hand_landmarks.landmark[connection[0]].x, hand_landmarks.landmark[connection[1]].x]
        y_values = [hand_landmarks.landmark[connection[0]].y, hand_landmarks.landmark[connection[1]].y]
        z_values = [hand_landmarks.landmark[connection[0]].z*(-1), hand_landmarks.landmark[connection[1]].z*(-1)]
        ax.plot(x_values, y_values, z_values, c='b')
        
    x, y, z = [], [], []
    for landmark in hand_landmarks.landmark:
        x = landmark.x
        y = landmark.y
        z = landmark.z*(-1)
        ax.scatter(x, y, z, c='r', marker='o')

def detectHandsLandmarks(image, hands):
    
    # Convert the image from BGR into RGB format.
    imageRGB = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Perform the Hands Landmarks Detection.
    results = hands.process(imageRGB)

    if results.multi_hand_landmarks:
        return results.multi_hand_landmarks[0]
    else:
        return None
    

# Inizializza la fotocamera
cap = cv2.VideoCapture(0)  # 0 indica la fotocamera principale, 1 per la secondaria

# Inizializza il rilevatore delle mani di MediaPipe
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=1,
                        min_detection_confidence=0.7, min_tracking_confidence=0.4)

# Example usage:
# Assuming you have a `landmarks` object with the required attributes
# Initialize the plot
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')    

while cap.isOpened():
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1)
    if not ret:
        break

    hand_landmarks = detectHandsLandmarks(frame, hands)
    if hand_landmarks:
        plot_world_landmarks(ax, hand_landmarks)
        plt.pause(0.1)
        plt.draw()

    # Tasto 'q' per uscire
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Rilascia le risorse
cap.release()
cv2.destroyAllWindows()
