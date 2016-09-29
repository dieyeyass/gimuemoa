<?php

if(isset($_POST["valider"])){
// première étape : désactiver le cache lors de la phase de test
ini_set("soap.wsdl_cache_enabled", "0");
// lier le client au fichier WSDL
$clientSOAP = new SoapClient('../gimonline_mpi.wsdl');
//ON RECUPERE LES PARAMETRES ENVOYES PAR LE FORMULAIRE ET ON Y AJOUTE LES AUTRES PARAMETRES
//NECESSAIRES AU BON FONCTIONNEMENT DU WEB SERVICE
$gol_site_id=$_POST['siteid'];
$gol_amount="";
$gol_currency="";
$gol_NumerosPanier="";
$gol_ctx_mode=$_POST['mode'];
$gol_page_action="CERTIFICAT";
$gol_trans_date="";
$gol_Signature="";
$idmarchand=$_POST['idmarchand'];
// executer la methode getMPIFunction pour récupérer le certificat
// généré par GIMUEMOA et on stocke le certificat dans une table coté client
// afin de rendre ce certificat permanent
$certificat=$clientSOAP->getMPIFunction($gol_site_id,$gol_ctx_mode,$gol_page_action);
//$pass_phrase= "TEST"."PAYMENT"."".$panier.$amount.$site_id."XOF".$reponse;
//$pass_signature= sha1($pass_phrase);
// SI LE CERTIFICAT EST RENVOYE ON L'ENREGISTRE DANS UNE TABLE COTE CLIENT
//SINON ON AFFICHE LE MESSAGE D'ERREUR SELON LE CODE D'ERREUR RECUPERE
if ($certificat !="5900"){
	include_once("concli.inc.php");
	$reqins=$connexion->prepare('INSERT INTO marchand(certificat,idmarchand,siteid)VALUES(:certificat,:idmarchand,:siteid)');
	$reqins->execute(array(
	                        'certificat'=>$certificat,
							'idmarchand'=>$idmarchand,
							'siteid'=>$gol_site_id
	                      )
	                );
	header('location:shipping.php');
}
else{
	$erreur="Veuillez revoir votre identifiant de site";
}

}
else{
?>
<html>
<head>
 <link rel="stylesheet" type="text/css" href="../css/bootstrap.min.css">
  <link rel="stylesheet" type="text/css" href="../css/gimclient.css">
</head>
<body >
 <div id="fondpage">
	<div id="datapayment">
	 <fieldset class="cadre">
	  <legend>PARAMETRAGES</legend>
	  <p style="color:red;font-weight:bold">Les champs en (*) sont obligatoires</p>
	  <!--p style="color:red;font-weight:bold"><?php //echo $erreur;?></p-->
	   <form method="POST" id="formu" action="gimclient.php" name="formulaire"  onSubmit="return check();">
	   <table>
	   <tr>
	    <td><label for="idmarchand">ID Marchand(*)</label></td>
		<td><input id="idmarchand" name="idmarchand" type="text" onKeyUp="javascript:couleur(this);"/></td>
	   </tr>
	   <tr>
		<td><label for="siteid">Identifiant du site(*)</label></td>
		<td><input id="siteid" name="siteid" type="text" onKeyUp="javascript:couleur(this);"/></td>
	   </tr>
	   <tr>
		<td><label for="mode">Mode(*)</label></td>
		<td><select id="mode" name="mode" type="text">
		      <option value="">--Choisir un mode--</option>
		      <option value="TEST">Test</option>
			  <option value="PRODUCTION">Production</option>
		    </select>
		</td>
	   </tr>	  
		</table></br>
		  <input type="submit" value=">Valider" name='valider' style="color:red"/>
		<input type="reset" value=">Annuler le parametrage" style="color:blue"/>
	   </form>
	   </div>
	 </fieldset>
	</div>
 
 </div>
<script type="text/javascript" src="../js/jquery.js"></script>
<script type="text/javascript" src="../js/bootstrap.min.js"></script>
<script type="text/javascript" src="../js/gimclient.js"></script>
</body>
</html>

<?php } ?>