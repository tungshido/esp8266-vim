const express = require('express');
const router = express.Router();
const dataQuery = require('../sql/cycleTimeQuery');
/* GET getData page. */
router.get('/:machineId', (req, res) => {
  const machineId = req.params.machineId;
  const startTime = req.query.startTime;
  const endTime = req.query.endTime;
  const cycleData = new Promise(resolve => {
    resolve(dataQuery.dataQuery(machineId, startTime, endTime));
  });
  cycleData.then(value => {
    console.timeEnd('query');
    console.log(value);
    res.send(value);
  });
});
router.post('/', (req, res) => {
  //console.log(req.body);
  if (req.body) {
    try {
      res.contentType('application/json');
      res.setHeader('Access-Control-Allow-Origin', '*');
    } catch (error) {
      console.log(error);
    }
  }
  return res.send('Post success');
});
module.exports = router;
