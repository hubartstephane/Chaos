
Import MaxGUI.Drivers

Local win:TGadget = CreateWindow("Virtual Resolution Test", 0, 0, 640, 480, Null, WINDOW_TITLEBAR | WINDOW_RESIZABLE | WINDOW_CLIENTCOORDS | WINDOW_CENTER)

Local canvas:TGadget = CreateCanvas(0, 0, 640, 480, win)
SetGadgetLayout canvas, EDGE_ALIGNED, EDGE_ALIGNED, EDGE_ALIGNED, EDGE_ALIGNED

Repeat
Select WaitEvent()
Case EVENT_APPTERMINATE
End
Case EVENT_WINDOWCLOSE
PostEvent CreateEvent(EVENT_APPTERMINATE)



Case EVENT_GADGETPAINT
SetGraphics CanvasGraphics(canvas)
Cls
SetColor Rand(0, 255), Rand(0, 255), Rand(0, 255)
DrawRect 16, 16, 608, 448
Flip


End Select

Until False