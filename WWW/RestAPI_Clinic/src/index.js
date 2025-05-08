require('dotenv').config();
const express = require('express');
const connectDB = require('./config/db');
// router import
const doctorsRouter      = require('./routes/doctors');
const patientsRouter     = require('./routes/patients');
const appointmentsRouter = require('./routes/appointments');

const authRouter = require('./routes/auth');
const authJWT = require('./middleware/authJWT');
const role = require('./middleware/role');

const errorHandler = require('./middleware/errorHandler');

const app = express();
app.use(express.json());

// Auth routes
app.use('/api/auth', authRouter);

// Protected routes example: only authenticated
app.use('/api/doctors', authJWT, doctorsRouter);
app.use('/api/patients', authJWT, patientsRouter);
app.use('/api/appointments', authJWT, appointmentsRouter);
// Only admin can delete appointments
app.use(errorHandler);

// // Global error handler
// app.use((err, req, res, next) => {
//   console.error(err);
//   res.status(err.status || 500).json({ error: err.message });
// });

connectDB().then(() => {
  const PORT = process.env.PORT || 3000;
  app.listen(PORT, () => console.log(`Server run on port ${PORT}`));
});

//tree */
//TODO
// /patient/id/appointments
// http status codes

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