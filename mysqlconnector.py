# -*- coding: utf-8 -*-
"""
Created on Sat Apr 30 21:33:55 2022

@author: vucri
"""

import mysql.connector, os 
cnn = mysql.connector.connect(host="127.0.0.1",user="root",passwd="9803",database="proyecto de grado")
'''

def consulta_ciudades():
    cur = cnn.cursor()
    cur.execute("SELECT * FROM acelerometros")
    datos = cur.fetchall()
    cur.close()    
    return datos  

'''
def abrir_txtdat():
    archivo = "datos.txt"
    lista_datos = []
    data_archivo = open(archivo,'r')
    
    for datos1 in data_archivo:
        
        lista_datos.append((datos1.strip()).split(sep=','))
    #print(f'data: {lista_datos}')
    #datos = lista_datos.readlines()
    #print(lista_datos[2])
    inserta_acelerometros(lista_datos)
    #print(type(data_archivo.read()))



def inserta_acelerometros(lista_datos):
    columns = list(zip(*lista_datos))
    datalista=()
    listacargar =[]
    for lista in lista_datos:
     
        #print(tuple(lista))
        cur = cnn.cursor()
        sql="INSERT INTO acelerometros (`FECHA Y HORA`,`NUMACELE`,`VALOR`)\
        VALUES(%s, %s, %s)"
        cur.executemany(sql,(tuple(lista),))
        #n=cur.rowcount
        #print(cur.fetchall())
        cnn.commit() 

    #cur.close()
    #return n 

    #print(listacargar)
    '''
    data1 = columns[0]
    data2 = columns[1]
    data3 = columns[2]
    for x,y,z in data1,data2,data3:
        print(x,y,z)
    
    cur = cnn.cursor()
    sql="INSERT INTO acelerometros (FECHA Y HORA,NUMACELE,VALOR)\
    VALUES(%s, %s, %s)"
    cur.executemany(sql,(data1,data2,data3))
    #n=cur.rowcount
    print(cur.fetchall())
    cnn.commit() 

    #cur.close()
    #return n  
  '''

if __name__ == "__main__":
    abrir_txtdat()