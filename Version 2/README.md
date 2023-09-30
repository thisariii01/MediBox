## Medibox Version 2.0 - Enhanced Medication Management and Light Control

Medibox Version 2.0 represents a significant upgrade, offering advanced features for medication management and light control. With the integration of a Node-RED dashboard, users can now interact with Medibox more intuitively. Key additions include:

#### Node-RED Dashboard: 
Medibox now features a Node-RED dashboard, providing a user-friendly interface to monitor and control the device. This dashboard simplifies interaction with the system and enhances user experience.

#### Light Intensity Measurement: 
Medibox utilizes a Light Dependent Resistor (LDR) to measure real-time light intensity. Users can monitor light levels via the Node-RED dashboard, which displays light intensity using a gauge for immediate feedback and a plot for visualizing historical variations.

#### Dynamic Light Regulation: 
To maintain optimal storage conditions for sensitive medications, a shaded sliding window is incorporated. This window is linked to a servo motor capable of adjusting its angle between 0-180 degrees based on lighting conditions. This intelligent control system dynamically regulates light entering the Medibox, preserving medication efficacy.

#### Customization: 
Recognizing that different medicines may require specific light conditions, Medibox Version 2.0 empowers users to tailor settings. Through the new group in the Node-RED dashboard, users can easily adjust the minimum angle and controlling factor for the shaded sliding window, ensuring precise control over the storage environment.
