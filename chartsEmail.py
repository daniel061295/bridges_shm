import mysql.connector
import numpy as np
from scipy.fft import fft, fftfreq
import matplotlib.pyplot as plt
from redmail import EmailSender
cnn = mysql.connector.connect(host="127.0.0.1",user="root",passwd="Manzana2132881+",database="proyecto")
mycursor = cnn.cursor()
mycursor.execute("SELECT VALOR FROM acelerometros WHERE NODO = 6")
myresult= mycursor.fetchall()
lista=[]
for row in myresult:
    lista.append(row[0])
#print(lista)
y = lista
N = len(y)
# N = 600
T = 0.100
x = np.linspace(0.0,N*T,N, endpoint=False)
# y = np.sin(80.0*2*np.pi*x) + np.sin(30.0*2*np.pi*x)
yf = fft(y)
xf = fftfreq(N,T)[:N//2]
fig1 = plt.figure()
plt.plot(x,y)
plt.grid()
#plt.show()
fig2 = plt.figure()
plt.plot(xf , 2.0/N * np.abs(yf[0:N//2]))
plt.grid()
email = EmailSender(host="smtp.gmail.com",port=587, username="daniel061295@gmail.com",password="xeqvbzbacijfpgco")
email.send(
    subject = "Graficas",
    sender = "danielcardenaswm@outlook.com",
    receivers = ["danielitocapa@hotmail.com","danielcardenaswm@outlook.com","daniel061295@gmail.com"],

    html = """
    <h1>En el tiempo</h1>
    {{ embedded_plot1}}
    <h1>En la frecuencia</h1>
    {{ embedded_plot2}}
    """,
    body_images= {"embedded_plot1":fig1,"embedded_plot2":fig2},
    attachments={"attached_plot1.png":fig1,"attached_plot2.png":fig2}
)