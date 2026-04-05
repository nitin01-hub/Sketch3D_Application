# Sketch3D_Application  
- A desktop application built with C++ and Qt6 that lets you sketch 2D shapes and instantly view them as extruded 3D objects. Features a full sketching canvas, shape editing tools, and an interactive OpenGL 3D viewer.

✏️ Sketch Mode
- Draw Triangles, Rectangles, and Squares on a 2D canvas
- Click and drag to create shapes of any size
- Select and move shapes freely on the canvas
- Delete individual shapes or clear the entire canvas

🎨 Shape Editing
- Color picker — assign any color to a selected shape
- Width & Height controls — resize shapes precisely
- Scale X / Scale Y — non-uniform scaling via the Properties panel
- Real-time updates as you adjust values

🧊 3D Viewing Mode
- All sketched shapes are automatically extruded into 3D solids
- Shapes are spread evenly in the 3D scene
- Click and drag to rotate the 3D view
- Scroll wheel to zoom in and out
- Auto-rotation animation when idle
- OpenGL lighting with top, side, and bottom face shading

🛠️ Built With
- C++17
- Qt 6 (Widgets, OpenGL, OpenGLWidgets)
- OpenGL / GLU — 3D rendering
- CMake — build system
- Qt Creator — IDE

📁 Project Structure

```
Sketch3D/
├── CMakeLists.txt       # Build configuration
├── main.cpp             # Application entry point
├── mainwindow.h/.cpp    # Main window, toolbar, properties panel
├── sketchwidget.h/.cpp  # 2D sketching canvas
├── shape.h/.cpp         # Shape data model (Triangle, Rectangle, Square)
└── viewer3D.h/.cpp      # OpenGL 3D viewer widget
```

<img width="1440" height="900" alt="Screenshot 2026-04-05 at 8 25 33 PM" src="https://github.com/user-attachments/assets/1a00fa7a-9826-4894-a677-d6a7ef8a4527" />
<img width="1440" height="900" alt="Screenshot 2026-04-05 at 8 25 38 PM" src="https://github.com/user-attachments/assets/8bac3150-b5fb-491e-810c-105ca4cd80f1" />

