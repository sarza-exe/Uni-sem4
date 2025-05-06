const express = require('express');
const router  = express.Router();
const doctorCtrl = require('../controllers/doctors');

// GET /doctors
router.get('/', doctorCtrl.getAll);

// GET /doctors/:id
router.get('/:id', doctorCtrl.getById);

// POST /doctors
router.post('/', doctorCtrl.create);

// PUT /doctors/:id
router.put('/:id', doctorCtrl.update);

// DELETE /doctors/:id
router.delete('/:id', doctorCtrl.remove);

module.exports = router;
