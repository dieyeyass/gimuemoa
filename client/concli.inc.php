<html>
<body>
<?php

include_once("param.inc.php");
$connexion=new PDO(DSN,USER,PASSWORD);
if(!$connexion){
?>	
<script type=text/javascript> 
alert('Connexion à la base impossible');
</script>
<?php
}
/*else{
	echo'Connexion reussie';
}*/

//$connexion->close();
?>

<body>

</html>