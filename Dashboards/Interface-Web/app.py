from flask import Flask, render_template
import pandas as pd
import plotly.express as px
import requests
import pickle

app = Flask(__name__)

@app.route('/') 
def index():
    # obter dados da API
    api_url = 'api_url_here'
    response = requests.get(api_url)
    data = response.json()

    df = pd.DataFrame(data['data'])  

    df['created_at'] = pd.to_datetime(df['created_at'])

    df['Tempo'] = df['created_at'].dt.strftime('%d-%m %H:%M:%S')

    #with open('modelo_knn.pkl', 'rb') as file:
    #    modelo_carregado = pickle.load(file)

    ultima_temperatura_int = df['temperature_int'].iloc[-1]
    ultima_temperatura_ext = df['temperature_ext'].iloc[-1]
    ultima_umidade_int = df['humidity_int'].iloc[-1]
    ultima_umidade_ext = df['humidity_ext'].iloc[-1]
    ultima_temperatura_alimento = df['temperature_alimento'].iloc[-1]

    #entrada = [ultima_temperatura_int, ultima_umidade_int, ultima_temperatura_ext, ultima_umidade_ext]
    # temperatura_prevista = modelo_carregado.predict([entrada])[0]
    # Formatando para exibir apenas dois dígitos após o ponto decimal
    #temperatura_prevista_formatada = f'{temperatura_prevista:.2f}'
    
    fig_temp = px.line(df, x="Tempo", y=["temperature_ext", "temperature_int", "temperature_alimento"], title="Temperatura Externa, Interna e do Alimento")
    fig_temp.update_xaxes(type="category")  # Usar categoria para o eixo x
    plot_temp_html = fig_temp.to_html(full_html=False)

    fig_umidade = px.line(df, x="Tempo", y=["humidity_int", "humidity_ext"], title="Umidade Interna e Externa")
    fig_umidade.update_xaxes(type="category")  # Usar categoria para o eixo x
    plot_umidade_html = fig_umidade.to_html(full_html=False)

    return render_template("index.html", 
                           plot_temp=plot_temp_html, 
                           plot_umidade=plot_umidade_html, 
                           ultima_temperatura_int=ultima_temperatura_int, 
                           ultima_temperatura_ext=ultima_temperatura_ext,
                           ultima_umidade_int=ultima_umidade_int,
                           ultima_umidade_ext=ultima_umidade_ext,
                           temperatura_atual_alimento = ultima_temperatura_alimento,
                           #temperatura_prevista = temperatura_prevista_formatada
                           )

if __name__ == '__main__':
    app.run(debug=True)
