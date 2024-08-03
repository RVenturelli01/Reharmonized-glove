'''
base function for the detection of the hand landmarks

'''

import cv2
import mediapipe as mp

# Inizializza la fotocamera
cap = cv2.VideoCapture(0)  # 0 indica la fotocamera principale, 1 per la secondaria

# Inizializza il rilevatore delle mani di MediaPipe
mp_hands = mp.solutions.hands
hands = mp_hands.Hands(static_image_mode=False, max_num_hands=2,
                        min_detection_confidence=0.7, min_tracking_confidence=0.4)

def detectHandsLandmarks(image, hands, display=True):
    '''
     This function performs hands landmarks detection on an image.
    Args:
        image:   The input image with prominent hand(s) whose landmarks needs to be detected.
        hands:   The hands function required to perform the hands landmarks detection.
        display: A boolean value that is if set to true the function displays the original input image
                 with hands landmarks drawn and returns nothing.
    Returns:
        results: The output of the hands landmarks detection on the input image.
    '''


    # Convert the image from BGR into RGB format.
    imageRGB = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)

    # Perform the Hands Landmarks Detection.
    results = hands.process(imageRGB)

    if display:
        # Disegna i landmark delle mani sul frame
        if results.multi_hand_landmarks:
            for hand_landmarks in results.multi_hand_landmarks:
                for lm in hand_landmarks.landmark:
                    # Traccia i landmark delle mani sul frame
                    h, w, c = frame.shape
                    cx, cy = int(lm.x * w), int(lm.y * h)
                    cv2.circle(frame, (cx, cy), 5, (255, 0, 0), cv2.FILLED)
        # Mostra il frame
        cv2.imshow('Hand Tracking', frame)
    else:
        return results


while cap.isOpened():
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1)
    if not ret:
        break

    detectHandsLandmarks(frame, hands, display=True)

    # Tasto 'q' per uscire
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Rilascia le risorse
cap.release()
cv2.destroyAllWindows()
