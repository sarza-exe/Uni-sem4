const express = require('express');
const router  = express.Router();
const patientCtrl = require('../controllers/patients');

// GET /patients
router.get('/', patientCtrl.getAll);

// GET /patients/:id
router.get('/:id', patientCtrl.getById);

// POST /patients
router.post('/', patientCtrl.create);

// PUT /patients/:id
router.put('/:id', patientCtrl.update);

// DELETE /patients/:id
router.delete('/:id', patientCtrl.remove);

module.exports = router;
