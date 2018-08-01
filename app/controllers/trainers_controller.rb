class TrainersController < ApplicationController

  def index

   if params[:query].present?
      @trainers = Trainer.all.near(params[:query],25)



  else
    @trainers = Trainer.all


@trainers = Trainer.where.not(latitude: nil,longitude: nil)

@markers = @trainers.map do |trainer|
 {
lat: trainer.latitude,
lng: trainer.longitude#,
# # infoWindow: { content: render_to_string(partial: "/flats/map_box", locals: { flat: flat }) }
}
end
end


end

 def show
  @trainer = Trainer.find(params[:id])
  @expertise = Expertise.find(@trainer.expertise_id)

end

def new
  @user = current_user
  @trainer = Trainer.new()
  @trainer.user = @user
  @expertise = Expertise.all
end

def create

  @user = current_user
  @trainer = Trainer.new(trainer_params)

 if @trainer.hourly_rate == nil
    flash.now[:alert] = 'please enter a hourly_rate-'
  end


  if @trainer.address == ""
    flash.now[:alert]  <<  'please enter an address-'
  end

 if @trainer.address2 == ""
    flash.now[:alert]  << 'please enter  a post code-'
  end

if @trainer.address2 == ""
    flash.now[:alert]  << 'please enter  a city-'
  end

if @trainer.first_name== ""
    flash.now[:alert]  << 'please enter your first name-'
  end

if @trainer.last_name== ""
    flash.now[:alert]  << 'please enter your last nam-'
  end

if @trainer.age= nil
    flash.now[:alert]  << 'please enter your age-'
  end

  if @trainer.photo= ""
    flash.now[:alert]  << 'please upload a photo'
  end

  if (@trainer.address != "")
  @trainer.address = @trainer.address + " " + @trainer.address2 + " " + @trainer.address3
  end

  @trainer.user = @user
  @expertise = Expertise.all

  if @trainer.save
    redirect_to trainer_path(@trainer)

  else
    render :new
  end

end

def edit


end

def update

end

def destroy

  @trainer = Trainer.find(params[:id])
  if (user_signed_in?)
  if @trainer.user_id == current_user.id
   @trainer.destroy
   redirect_to trainers_path
  end
 end
end

private
def trainer_params


  params.require(:trainer).permit(:expertise_id, :hourly_rate, :photo, :address,:address2,:address3,:first_name,
    :last_name, :age, :gender)

 end


end




