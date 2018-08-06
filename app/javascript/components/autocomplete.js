// app/javascript/components/autocomplete.js
function autocomplete() {
  document.addEventListener("DOMContentLoaded", function() {
    var trainerAddress = document.getElementById('find');

    if (trainerAddress) {
      var autocomplete = new google.maps.places.Autocomplete(trainerAddress, { types: [ 'geocode' ] });
      google.maps.event.addDomListener(trainerAddress, 'keydown', function(e) {
        if (e.key === "Enter") {
          e.preventDefault(); // Do not submit the form on Enter.
        }
      });
    }
  });
}

export { autocomplete };

