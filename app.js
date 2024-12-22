// Import Firebase
import { initializeApp } from "https://www.gstatic.com/firebasejs/9.6.1/firebase-app.js";
import {
  getDatabase,
  ref,
  onValue,
} from "https://www.gstatic.com/firebasejs/9.6.1/firebase-database.js";

// Firebase configuration, replace these credentials with yours
const firebaseConfig = {
  apiKey: "************************************",
  authDomain: "************************************",
  databaseURL: "************************************",
  projectId: "************************************",
  storageBucket: "************************************",
  messagingSenderId: "************************************",
  appId: "************************************",
  measurementId: "************************************",
};

// Initialize Firebase
const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

// Function to retrieve and display the soil moisture data from Firebase
function displaySoilMoisture() {
  const moistureRef = ref(database, "SoilMoisture"); // Path for soil moisture data
  onValue(
    moistureRef,
    (snapshot) => {
      const moistureLevel = snapshot.val();
      if (moistureLevel !== null) {
        console.log("Retrieved Moisture Level:", moistureLevel); // Log the value
        document.getElementById(
          "moistureLevel"
        ).textContent = `Soil Moisture Level: ${moistureLevel}%`;
      } else {
        console.error("No moisture data found in Firebase.");
      }
    },
    (error) => {
      console.error("Error reading moisture data from Firebase:", error);
    }
  );
}
// <------------------------------------------------------------------------>
function displayHumidiy() {
  const humidRef = ref(database, "Humidity");
  onValue(
    humidRef,
    (snapshot) => {
      const HumidityLevel = snapshot.val();
      if (HumidityLevel !== null) {
        console.log("Retrieved Humid level: ", HumidityLevel);
        document.getElementById(
          "humiditylevel"
        ).textContent = `Humidity level: ${HumidityLevel}%`;
      } else {
        console.error("No humidity data found in Firebase");
      }
    },
    (error) => {
      console.error("Error reading humid data from Firebase: ", error);
    }
  );
}

// <-------------------------------------------------------------------------->

// Call the function to display moisture data
displaySoilMoisture();

// Optional: Function to update the date and time in HTML
function updateDateTime() {
  const now = new Date();
  const dateString = now.toLocaleDateString();
  const timeString = now.toLocaleTimeString();
  document.getElementById(
    "dateTime"
  ).textContent = `Date: ${dateString}, Time: ${timeString}`;
}

// Update the date and time every second
setInterval(updateDateTime, 1000);
