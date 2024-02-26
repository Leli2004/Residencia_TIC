# Importando bibliotecas necessárias
import numpy as np
import matplotlib.pyplot as plt
from sklearn.ensemble import RandomForestRegressor
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error

# Gerando dados de exemplo
# Suponha que a relação entre temperatura externa e interna seja uma função cúbica
np.random.seed(0)
X = np.sort(5 * np.random.rand(80, 1), axis=0)  # Temperatura externa
y = np.sin(X).ravel() + np.random.normal(0, 0.1, X.shape[0])  # Temperatura interna

# Dividindo os dados em treinamento e teste
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=42)

# Treinando o modelo
regr = RandomForestRegressor(n_estimators=100, random_state=42)
regr.fit(X_train, y_train)

# Fazendo previsões
y_pred = regr.predict(X_test)

# Avaliando o modelo
mse = mean_squared_error(y_test, y_pred)
print(f"Erro Médio Quadrático: {mse:.4f}")

# Plotando os resultados
plt.figure()
plt.scatter(X, y, s=20, edgecolor="black", c="darkorange", label="dados")
plt.plot(X_test, y_pred, color="cornflowerblue", label="previsão", linewidth=2)
plt.xlabel("Temperatura Externa")
plt.ylabel("Temperatura Interna")
plt.title("Regressão usando Random Forest")
plt.legend()
plt.show()
