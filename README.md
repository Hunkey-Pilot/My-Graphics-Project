A visually rich 2D animated town simulation built using OpenGL (FreeGLUT) in C++.
This project demonstrates computer graphics concepts including transformations, animation, basic algorithms, and interactive scene control.

📌 Features
🌤️ Multiple Scenarios
Clear Day
Rainy Day
Night
Rainy Night

🚗 Animated Car System
Smooth acceleration & deceleration
Traffic signal interaction (Red, Yellow, Green)

🚦 Traffic Signal Logic
Realistic stopping behavior
Automatic transition (Red → Yellow → Green)

☁️ Dynamic Clouds
Multi-layer cloud movement for depth effect

🌧️ Rain Animation
Randomized rain drops
Works in both day and night modes

🌊 Water Reflection System
Reflection of:
Car
Traffic light
Boat

⛵ Boat Animation
Moving boat with reflection

🌳 Environment Design
Houses, trees, bushes
Roads, footpaths, pond
Street lights

🌙 Day/Night Effects
Sun and Moon rendering
Stars at night
Light glow changes

🔊 Sound Effects
Bird sound (day)
Rain sound (rainy scenes)
Night ambience (crickets)


🧠 Algorithms Used
DDA Line Drawing Algorithm
Midpoint Circle Algorithm
Basic transformations:
Translation
Scaling
Reflection
🎮 Controls

Key	Action
D	Clear Day
W	Rainy Day
N	Night
M	Rainy Night
R	Traffic Light → Red
G	Traffic Light → Yellow → Green

🛠️ Technologies Used
C++
OpenGL
FreeGLUT
Windows Multimedia API (for sound)

📂 Project Structure
Town-Scenario/
│── main.cpp
│── assets/
│   ├── birds-sfx.wav
│   ├── thunder-rain.wav
│   ├── night-atmosphere.wav
│── README.md

▶️ How to Run

🧾 Requirements
C++ Compiler (MinGW / MSVC)
OpenGL
FreeGLUT

⚙️ Compile & Run (MinGW Example)
g++ myproject.cpp -o urban -lfreeglut -lopengl32 -lglu32 -lwinmm
./urban

🎯 Learning Outcomes
Understanding of classic graphics algorithms
Real-time animation using timers
Scene management in OpenGL
Interactive input handling
Sound integration in C++

⚠️ Limitations
Fixed screen resolution (not responsive)
Hardcoded object positions
No texture mapping (basic color rendering)
Limited physics realism

🚀 Future Improvements
Add user-controlled vehicle movement
Implement collision detection
Introduce textures for realistic visuals
Add more weather effects (fog, storm, etc.)
Convert to modern OpenGL (Shaders)

👨‍💻 Author
Anupam Pal
B.Sc in Computer Science & Engineering (CSE)
Daffodil International University

📜 License

This project is for educational purposes.Any one can use only for Educational Purpose. 
