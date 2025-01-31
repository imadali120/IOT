import { initializeApp } from "https://www.gstatic.com/firebasejs/9.1.1/firebase-app.js";
import { getDatabase, ref, get, set } from "https://www.gstatic.com/firebasejs/9.1.1/firebase-database.js";

const firebaseConfig = {
  apiKey: "AIzaSyCgtEy05JPROdpvGLe0hfQu_Z5_FZUxSxI",
  authDomain: "esp32-security-67b5b.firebaseapp.com",
  databaseURL: "https://esp32-security-67b5b-default-rtdb.europe-west1.firebasedatabase.app",
  projectId: "esp32-security-67b5b",
  storageBucket: "esp32-security-67b5b.firebasestorage.app",
  messagingSenderId: "563136402646",
  appId: "1:563136402646:web:89bd928de03936bddba577"
};

const app = initializeApp(firebaseConfig);
const database = getDatabase(app);

const ldrStateElement = document.getElementById("ldrState");
const distanceElement = document.getElementById("distance");
const toggleSensorBtn = document.getElementById("toggleSensorBtn");
const sensorStateDiv = document.getElementById("sensorState");
// const sosButton = document.getElementById("sosButton");
// const sosRef = ref(database, "/sos");
// const sirenButton = document.getElementById("sirenButton");
// const sirenRef = ref(database, "/siren"); // Reference to the 'siren' boolean in Firebase
//
// function updateSirenButton() {
//   get(sirenRef).then((snapshot) => {
//     sirenButton.textContent = snapshot.val() ? "Deactivate Siren" : "Activate Siren";
//   });
// }
//
// updateSirenButton(); // Initial update
//
// sirenButton.addEventListener("click", () => {
//   get(sirenRef).then((snapshot) => {
//     const currentSiren = snapshot.val();
//     set(sirenRef, !currentSiren)
//         .then(() => {
//           console.log("Siren updated in Firebase");
//           updateSirenButton();
//         })
//         .catch((error) => {
//           console.error("Error updating siren:", error);
//         });
//   });
// });

function updateSensorData() {
  const sensorStateRef = ref(database, "/sensor/state");
  const ldrRef = ref(database, "/sensor/data/ldr");
  const distanceRef = ref(database, "/sensor/data/distance");

  get(sensorStateRef).then((snapshot) => {
    const sensorState = snapshot.val();
    toggleSensorBtn.textContent = sensorState === "on" ? "Turn Off Sensor" : "Turn On Sensor";
    sensorStateDiv.querySelector("h2").textContent = sensorState === "on" ? "Current Readings in the Room" : "Last Readings in the Room";
  });

  get(ldrRef).then((snapshot) => {
    ldrStateElement.textContent = snapshot.val() === "Bright" ? "Bright" : "Dark";
  });

  get(distanceRef).then((snapshot) => {
    distanceElement.textContent = `${snapshot.val()} cm`;
  });
}

toggleSensorBtn.addEventListener("click", () => {
  const sensorStateRef = ref(database, "/sensor/state");
  get(sensorStateRef).then((snapshot) => {
    const sensorState = snapshot.val();
    set(sensorStateRef, sensorState === "on" ? "off" : "on");
    updateSensorData();
  });
});

// Function to update the SOS button text based on Firebase value
// function updateSOSButton() {
//   get(sosRef).then((snapshot) => {
//     sosButton.textContent = snapshot.val() ? "Deactivate SOS" : "Activate SOS";
//   });
// }
//
// // Initial update of the SOS button
// updateSOSButton();
//
// sosButton.addEventListener("click", () => {
//   get(sosRef).then((snapshot) => {
//     const currentSOS = snapshot.val();
//     set(sosRef, !currentSOS)
//         .then(() => {
//           console.log("SOS updated in Firebase");
//           updateSOSButton();
//         })
//         .catch((error) => {
//           console.error("Error updating SOS:", error);
//         });
//   });
// });

updateSensorData();
setInterval(updateSensorData, 2000);