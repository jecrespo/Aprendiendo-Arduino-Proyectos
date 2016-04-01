Public Class Form1
    Private Sub Form1_Load(sender As Object, e As EventArgs) Handles MyBase.Load
        GetSerialPortNames()
    End Sub
    'Muestra los puertos serie disponibles
    Public Sub GetSerialPortNames()
        For Each sp As String In My.Computer.Ports.SerialPortNames
            ComboBox1.Items.Add(sp)
        Next
    End Sub
    Private Sub Button1_Click(sender As Object, e As EventArgs) Handles Button1.Click
        If Button1.Text = "Conecta" Then
            Button1.Text = "Desconecta"
            SerialPort1.PortName = ComboBox1.SelectedItem
            SerialPort1.Open()
            Dim s As String = ""
            For i As Integer = 1 To 1000
                s = SerialPort1.ReadLine & Chr(13) & Chr(10) & s
                TextBox1.Text = s
            Next
        ElseIf Button1.Text = "Desconecta" Then
            Button1.Text = "Conecta"
            SerialPort1.Close()
        End If
    End Sub

End Class
