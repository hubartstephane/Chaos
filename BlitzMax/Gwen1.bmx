Graphics 800, 600

SetClsColor 255, 159, 78

Local Image:TImage = LoadImage("voiture.png")


Local diametre = 30

Local X = 0
While Not AppTerminate()

	Cls
	
		DrawImage image, X, 100

		x = x + 1
		If x > 800 Then 
			x = 0
		End if

	Flip
	


Wend