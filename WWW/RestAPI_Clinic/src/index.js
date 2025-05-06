require('dotenv').config();
const express = require('express');
const connectDB = require('./config/db');

const app = express();
app.use(express.json());

// router import
const doctorsRouter      = require('./routes/doctors');
const patientsRouter     = require('./routes/patients');
const appointmentsRouter = require('./routes/appointments');

// Uruchomienie serwera po udanym połączeniu
connectDB().then(() => {
  app.listen(process.env.PORT || 3000, () =>
    console.log(`Server listening on port ${process.env.PORT || 3000}`)
  );
});

//tree */

// clinic-api/
// ├─ src/
// │  ├─ config/        # plik do ładowania .env i łączenia z DB
// │  ├─ models/        # schematy Mongoose lub definicje Sequelize
// │  ├─ routes/        # pliki z routingiem dla doctors, patients, appointments
// │  ├─ controllers/   # logika obsługi żądań (CRUD + paginacja/filtry)
// │  ├─ middleware/    # np. authJWT.js, errorHandler.js
// │  └─ index.js       # główny serwer Express
// └─ .env

// TODO
// Modele – zaimplementuj schematy Doctor, Patient, Appointment.

// Routing – stwórz pliki routes/doctors.js, routes/patients.js, routes/appointments.js, załaduj je w index.js.

// Kontrolery – w controllers/ zrób CRUD i w jednym z zasobów paginację/filtrowanie/sortowanie (np. lista wizyt z filtrem po dacie i paginacją).

// Auth – middleware do wyciągnięcia i weryfikacji JWT, funkcje logowania (POST /auth/login) i rejestracji.

// Role – w middleware sprawdzaj req.user.role, blokuj nieautoryzowane ścieżki.