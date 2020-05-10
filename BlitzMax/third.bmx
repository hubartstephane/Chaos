
' if bpp provided => Fullscreen
Graphics 800,600

'Incbin "Sprite.png" ' the resource is embedded in the EXE

'SetMaskColor 255,255,255 'The color that will be used for transparent in the image

'Local Img:TImage = LoadImage("incbin::Sprite.png")
SetAudioDriver("FreeAudio SDL")

Local Img:TImage = LoadImage("Sprite.png")

Local x = MouseX()

Local Snd:TSound = LoadSound("drum.wav")

'MidHandleImage(Img)
'AutoMidHandle(True)

'LoadAnimImage("path", width, height, start, count)

'SetColor(255, 255, 255)
'SetAlpha(0.5)
SetScale(2.0, 2.0)

Const CONSTANT# = 13

Type TT Abstract

End Type


Type T

'Rem

	Method operator:+(other:T)
		Return 3
	
	End Method
'End Rem

	Public Method New()
	
		Print "Hello"
	
	End Method

End Type


'New T + New T

' MoveMouse

If Img Then
	Print "Image charge"
End If

If Snd Then
	Print "Sound charge"
End If

'SetColor 128, 0, 0

SetClsColor 0, 0, 255

While Not KeyDown(Key_Escape)

	If KeyDown(Key_Space) Then
		If Snd
			Snd.Play()
		End If
		End
	End If

	Local frame = 0

	If MouseDown(1) Then
		DrawRect 500, 500, 100, 100
	End If

	'If KeyHit(Key_Space) Then
	If KeyDown(Key_Space) Then
		DrawImage Img, 500, 500, frame
	End If

	DrawText "FPS : " + MilliSecs(), 100, 100
	
	DrawText "Width  : " + GraphicsWidth(), 100, 150
	DrawText "Height : " + GraphicsHeight(), 100, 180

	Flip
	Cls

Wend


While Not KeyDown(Key_Escape)

Flip
CLS

Wend