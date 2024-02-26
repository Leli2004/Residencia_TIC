import numpy as np
import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.linear_model import LinearRegression
from sklearn.metrics import mean_squared_error, r2_score

# Coleta dos dados da API

# Entrada dos dados para a regressão
data = {
    'temperature_int': [0.55,0.72,0.6,0.54,0.42,0.65,0.44,0.89,0.96,0.38,0.79,0.53,0.57,0.93,0.07],
    'humidity_int': [0.09,0.02,0.83,0.78,0.87,0.98,0.8,0.46,0.78,0.12,0.64,0.14,0.94,0.52,0.41],
    'temperature_ext': [0.26,0.77,0.46,0.57,0.02,0.62,0.61,0.62,0.94,0.68,0.36,0.44,0.7,0.06,0.67],
    'humidity_ext': [0.67,0.21,0.13,0.32,0.36,0.57,0.44,0.99,0.1,0.21,0.16,0.65,0.25,0.47,0.24],
    'temperature_alimento': [0.16,0.11,0.66,0.14,0.2,0.37,0.82,0.1,0.84,0.1,0.98,0.47,0.98,0.6,0.74]
}

# Cria um dataframe a partir dos dados
df = pd.DataFrame(data)

# Define quais serão as variáveis de entrada e saída, sendo X = entrada e Y = saída
x = df[['temperature_int', 'humidity_int', 'temperature_ext', 'humidity_ext']]
y = df['temperature_alimento']

# Divide os dados entre os dados de treinamento e de teste
teste_porcent = 0.2 # Porcentagem da base de dados que será utilizada no teste do modelo
x_treino, x_teste, y_treino, y_teste = train_test_split(x, y, test_size=teste_porcent, random_state=0)

# Criar o modelo de regressão linear
modelo = LinearRegression()

# Treinar o modelo com os dados de treino
modelo.fit(x_treino, y_treino)

# Fazer previsões com os dados de teste
y_predicao = modelo.predict(x_teste)

# Média do erro ao quadrado: 
# Quanto mais próximo de zero, mais preciso o modelo
mse = mean_squared_error(y_teste, y_predicao) 

# R² (Coeficiente de determinação): 
# Varia de 0 a 1, quanto maior o R², melhor o modelo em termos de explicar as variações na variável dependente.
r2 = r2_score(y_teste, y_predicao)

print(f'Média do ero ao quadrado: {mse}')
print(f'R^2: {r2}')

# Gera a função da regressão linear múltipla

print(f'Intercept (b0): {modelo.intercept_}')
for i, coef in enumerate(modelo.coef_):
    print(f'Coeficiente b{i+1} (para X{i+1}): {coef}')


