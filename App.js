


import { createDrawerNavigator } from '@react-navigation/drawer'; // Import createDrawerNavigator
import { NavigationContainer } from '@react-navigation/native';
import { createStackNavigator } from '@react-navigation/stack';
import React from 'react';
import { StyleSheet, View } from 'react-native';
import First from '../MainProject/Hydro/screens/First.js';
import Home from '../MainProject/Hydro/screens/Home.js';
import LoginScreen from '../MainProject/Hydro/screens/LoginScreen.js';
import Weather from '../MainProject/Hydro/screens/Weather.js';

const Stack = createStackNavigator();
const Drawer = createDrawerNavigator(); // Create Drawer navigator instance

export default function App() {
  return (
    <NavigationContainer>
      
      <View style={styles.container}>
        <Stack.Navigator>
          <Stack.Screen
            name="Chauhan's Project"
            component={First}
            options={{ headerShown: false }}
          />
          <Stack.Screen
            name="Weather"
            component={Weather}
            options={{
              headerShown:false
            }}
          />
          
          <Stack.Screen
            name="LoginScreen"  
            component={LoginScreen}
            options={{ headerShown: false }}
          />
          <Stack.Screen
            name="Home"  
            component={Home}
            options={{ headerShown: false }}
          />
        </Stack.Navigator>
      </View>
    </NavigationContainer>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fff',
  },
});
