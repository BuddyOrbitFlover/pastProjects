import pandas as pd
from sklearn.model_selection import train_test_split
from sklearn.ensemble import RandomForestRegressor
from sklearn.metrics import mean_absolute_error, mean_squared_error, r2_score

# Load and preprocess data
flight_data = pd.read_csv("flight_data.csv")

# Sample 20% of the data
flight_data_sample = flight_data.sample(frac=0.2, random_state=19)

columns_to_drop = ['ID', 'Base', 'Flight', 'ShortFlightDate', 'CategoricalDepartureTime', 
                   'OriginCountryCode', 'DestinationCountryCode', 'HolidayInOrigin', 
                   'HolidayInDestination', 'IsOutbound', 'Capacity Physical Y', 
                   'Capacity Saleable Y', 'Year', 'Day', 'WeekNum', 'dayofweek', 
                   'day_of_week']
flight_data_sample = pd.get_dummies(flight_data_sample.drop(columns_to_drop, axis=1), 
                            columns=['Origin', 'Destination', 'FlightDateTime', 
                                     'FlightDurationInHours', 'Boarded Y', 'Month'])

# Split data
X = flight_data_sample.drop(['Quantity'], axis=1)
y = flight_data_sample['Quantity']
X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=19)

# Train model
rfr = RandomForestRegressor(random_state=13)
rfr.fit(X_train, y_train)
y_pred = rfr.predict(X_test)

# Evaluate model
mae = mean_absolute_error(y_test, y_pred)
mse = mean_squared_error(y_test, y_pred)
r2 = r2_score(y_test, y_pred)

print("Mean Absolute Error:", mae)
print("Mean Squared Error:", mse)
print("R-squared:", r2)


# # Load flight data
# df = pd.read_csv('flight_data.csv')

# # Filter for BRU departures
# df_bru_departures = df[df['Origin'] == 'BRU']

# # Preprocess data
# df_bru_departures = df_bru_departures.drop(['ID', 'Base', 'Flight', 'FlightDateTime'], axis=1)

# # Handle CategoricalDepartureTime (assuming it contains strings like "Early Morning")
# df_bru_departures['CategoricalDepartureTime'] = df_bru_departures['CategoricalDepartureTime'].astype('category').cat.codes

# # One-hot encode Destination and day_of_week
# df_bru_departures = pd.get_dummies(df_bru_departures, columns=['Destination', 'day_of_week'])

# # Extract date features
# df_bru_departures['ShortFlightDate'] = pd.to_datetime(df_bru_departures['ShortFlightDate'])
# df_bru_departures['Year'] = df_bru_departures['ShortFlightDate'].dt.year
# df_bru_departures['Month'] = df_bru_departures['ShortFlightDate'].dt.month
# df_bru_departures['Day'] = df_bru_departures['ShortFlightDate'].dt.day
# df_bru_departures = df_bru_departures.drop(['Origin', 'DestinationCountryCode', 'ShortFlightDate'], axis=1)

# # Split into features and target
# X = df_bru_departures.drop('Quantity', axis=1)
# y = df_bru_departures['Quantity']

# # Train Random Forest model
# model = RandomForestRegressor(n_estimators=100, random_state=42)
# model.fit(X, y)

# # Generate predictions for all possible routes
# possible_destinations = df['Destination'].unique()
# predictions_df = pd.DataFrame(columns=['Destination', 'Predicted_Sandwiches'])

# for destination in possible_destinations:
#     X_destination = X.copy()
#     X_destination[f'Destination_{destination}'] = 1  # Set destination column
#     predicted_sandwiches = model.predict(X_destination)
#     predictions_df = predictions_df.append({'Destination': destination,
#                                             'Predicted_Sandwiches': predicted_sandwiches.mean()},
#                                            ignore_index=True)

# # Print predictions
# print(predictions_df)
