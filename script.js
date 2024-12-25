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


function updateSensorData() {
  const sensorStateRef = ref(database, "/sensor/state");
  const ldrRef = ref(database, "/sensor/data/ldr");
  const distanceRef = ref(database, "/sensor/data/distance");


  get(sensorStateRef).then((snapshot) => {
    const sensorState = snapshot.val();
    if (sensorState === "on") {
      toggleSensorBtn.textContent = "Turn Off Sensor";
      sensorStateDiv.querySelector("h2").textContent = "Current Readings";
    } else {
      toggleSensorBtn.textContent = "Turn On Sensor";
      sensorStateDiv.querySelector("h2").textContent = "Last Readings";
    }
  });


  get(ldrRef).then((snapshot) => {
    const ldrValue = snapshot.val();
    ldrStateElement.textContent = ldrValue === "Bright" ? "Bright" : "Dark";
  });


  get(distanceRef).then((snapshot) => {
    const distance = snapshot.val();
    distanceElement.textContent = `${distance} cm`;
  });
}


toggleSensorBtn.addEventListener("click", () => {
  const sensorStateRef = ref(database, "/sensor/state");


  get(sensorStateRef).then((snapshot) => {
    const sensorState = snapshot.val();
    const newState = sensorState === "on" ? "off" : "on";
    set(sensorStateRef, newState);
    updateSensorData();
  });
});


updateSensorData();


setInterval(updateSensorData, 2000);